#include "shape_file.h"

#include <QFile>
#include <QTextStream>
#include <memory>
#include <vector>

#include "my_editor.h"
#include "shape.h"
#include "shape_registry.h"

bool SaveShapes(const MyEditor& editor, const QString& path, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (error)
            *error = "Не вдалося відкрити файл для запису: " + file.errorString();
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "# Lab5: множина графічних об'єктів\n";
    out << "# формат рядка: <тип> <x1> <y1> <x2> <y2>\n";
    for (int i = 0; i < editor.Count(); ++i) {
        const Shape& s = editor.ShapeAt(i);
        out << s.Id() << ' ' << s.X1() << ' ' << s.Y1() << ' ' << s.X2() << ' ' << s.Y2() << '\n';
    }
    out.flush();

    if (out.status() != QTextStream::Ok || file.error() != QFile::NoError) {
        if (error)
            *error = "Помилка запису файлу: " + file.errorString();
        return false;
    }
    return true;
}

bool LoadShapes(MyEditor& editor, const QString& path, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error)
            *error = "Не вдалося відкрити файл: " + file.errorString();
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    std::vector<std::unique_ptr<Shape>> loaded;
    int lineNo = 0;
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        ++lineNo;
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        const QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        bool ok = parts.size() == 5;
        int v[4] = {0, 0, 0, 0};
        for (int i = 0; ok && i < 4; ++i)
            v[i] = parts[i + 1].toInt(&ok);
        if (!ok) {
            if (error)
                *error = QString("Рядок %1: очікується «тип x1 y1 x2 y2».").arg(lineNo);
            return false;
        }

        const ShapeInfo* info = FindShapeInfo(parts[0].toLatin1().constData());
        if (info == nullptr) {
            if (error)
                *error = QString("Рядок %1: невідомий тип фігури «%2».").arg(lineNo).arg(parts[0]);
            return false;
        }
        if (static_cast<int>(loaded.size()) >= MyEditor::N) {
            if (error)
                *error = QString("У файлі більше %1 фігур — стільки редактор вмістити не може.")
                             .arg(MyEditor::N);
            return false;
        }

        std::unique_ptr<Shape> shape(info->create());
        shape->Set(v[0], v[1], v[2], v[3]);
        loaded.push_back(std::move(shape));
    }

    editor.Clear();
    for (auto& shape : loaded)
        editor.AddShape(shape.release());
    return true;
}