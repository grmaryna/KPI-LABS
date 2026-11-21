# Лабораторна робота 5: Нормалізація бази даних

**Мета:** покращити цілісність даних та усунути їхню надлишковість шляхом покрокової нормалізації існуючої схеми бази даних до Третьої нормальної форми (3НФ), щоб кожен факт зберігався лише один раз без аномалій вставки, оновлення та видалення.

**Цілі:**
- виявити потенційну надлишковість даних та аномалії (вставки, оновлення, видалення) у початковій схемі БД;
- визначити та задокументувати повний мінімальний набір функціональних залежностей (ФЗ) для таблиць;
- oцінити поточну нормальну форму (1NF, 2NF, 3NF) кожної таблиці на основі її ключів та ФЗ;
- провести покрокову декомпозицію таблиць до 3НФ (усунувши повторювані групи, часткові та транзитивні залежності);
- оновити SQL-скрипти (`CREATE TABLE` / `ALTER TABLE`) у `PostgreSQL`, оновити ER-діаграму та перевірити цілісність даних на тестових прикладах.

## Перехід до 1НФ

### Проблема 01: `CustomerName` за вимогами має містити прізвище та імʼя, а це не відповідає критерію атомарності атрибутів.

**Попередня таблиця:** 

![customer table](img/customer-table-old.png)

**Кроки змін:**

1. Перейменування `CustomerName` на `CustomerFirstName`

![rename customer name](img/rename-customer-name.png)

![result rename customer name](img/result-rename-customer-name.png)

2. Додали `CustomerLastName`

![add customer last name](img/add-customer-last-name.png)

![result add customer last name](img/res-add-customer-last-name.png)

3. Додали прізвища всім наявним клієнтам в базі

![add last name](img/add-last-name.png)

![result add last name](img/res-add-last-name.png)

4. Задали обмеження для атрибуту `CustomerLastName` - `not null`

![set last name not null](img/lastname-not-null.png)

### Проблема 02: `OrderDeliveryPlace` за вимогами має містити країну та поштовий індекс, а це не відповідає критерію атомарності атрибутів.

**Попередня таблиця:** 

![order table](img/old-order.png)

**Кроки змін:**

1. Додали `PostCode`

![add post code](img/add-post-code.png)

![result add post code](img/result-add-post-code.png)

2. Додали `PostCode` всім наявним клієнтам в базі

![add post code](img/post-code.png)

![result add post code](img/res-add-post-code.png)

2. `OrderDeliveryPlace` за логікою стало країною доставки, тоу оновили дані

![Order Delivery Place](img/order-place.png)

![Result Order Delivery Place](img/res-order-place.png)

## Перехід до 2НФ

### Критерії 2NF

1. Таблиця знаходиться в 1NF
2. Кожен неключовий атрибут повністю функціонально залежить від первинного ключа (немає часткових залежностей)

### Аналіз поточних таблиць

1. Таблиці `Customer(PK: CustomerID)`, `Good(PK: GoodID)`, `Order (PK: OrderID)`, `Payment(PK: PaymentID)` мають первинний ключ, який складається лише з одного стовпця, тому часткової залежності тут не може бути в апріорі.

2. Таблиця `OrderItem` (Складений ключ: `OrderID, GoodID`)

Неключові атрибути: `Quantity`, Price`A`tPurchase
* Перевірка
    * `Quantity` залежить від двох частин складеного ключа тому, що одне замовлення може містити товари з різною кількістю, і один і той самий товар може бути у різних замовленнях у різній кількості. 
    * `PriceAtPurchase` залежить від двох частин складеного ключа тому, що ціна залежить від самого товару, а також від замовлення та кількості товару на момент замовлення

3. Таблиця `Review` (Складений ключ: `CustomerID`, `GoodID`)

Неключові атрибути: `ReviewText`, `ReviewGrade`
* Перевірка
    * `ReviewText` залежить від двох частин ключа тому, що клієнт пише різні відгуки на різні товари, і відгуки можуть бути написані різними людьми
    * `ReviewGrade` залежить від двох частин ключа тому, що різні клієнти пишуть різні відгуки на різні товари

## Перехід до 3НФ

### Проблема 01: Поле `OrderPrice` є надлишковим, оскільки його можна обчислити.

**Попередня таблиця:** 

![customer table](img/order-item.png)

**Кроки змін:**

1. Видаляєм `OrderPrice`

![drop price](img/drop-price.png)

![result drop price](img/res-drop-price.png)

## Зміни в діаграмі

Діаграмі до змін:

![old-diagram](img/old-diagram.png)

Діаграма після змін:

![new-diagram](img/new-diagram.png)

## Функціональні залежності

CustomerID -> CustomerFirstName, CustomerLastName, CustomerPhone, CustomerEmail, CustomerPassword // Повна залежність

GoodID -> GoodName, GoodPrice, GoodStatus, GoodDescription, GoodQuantity // Повна залежність

OrderID -> CustomerID, OrderDeliveryPlace, Postcode // Повна залежність 

PaymentID -> OrderID, PaymentPrice, PaymentDate, PaymentMethod, PaymentStatus // Повна залежність

(OrderID, GoodID) -> Quantity, PriceAtPurchase // Повна залежність

(CustomerID, GoodID) -> ReviewText, ReviewGrade // Повна залежність