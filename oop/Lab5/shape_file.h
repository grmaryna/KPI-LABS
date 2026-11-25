#pragma once

#include <QString>

class MyEditor;

bool SaveShapes(const MyEditor& editor, const QString& path, QString* error);

bool LoadShapes(MyEditor& editor, const QString& path, QString* error);