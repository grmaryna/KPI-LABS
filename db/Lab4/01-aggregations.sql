-- Запит 1.1: загальна фіансова статистика

SELECT 
    COUNT(*) AS Total_Orders,
    SUM(OrderPrice) AS Total_Revenue,
    AVG(OrderPrice) AS Average_Order_Price,
    MAX(OrderPrice) AS Most_Expensive_Order,
    MIN(OrderPrice) AS Cheapest_Order
FROM public.order;

SELECT 
    PaymentMethod,
    COUNT(*) AS number_of_transactions,
    SUM(PaymentPrice) AS total_sum
FROM Payment
GROUP BY PaymentMethod;

SELECT 
    CustomerID,
    COUNT(OrderID) AS Number_of_Orders,
    SUM(OrderPrice) AS Total_Spent
FROM public.order
GROUP BY CustomerID;

SELECT 
    CustomerID,
    COUNT(OrderID) AS Number_of_Orders
FROM public.order
GROUP BY CustomerID
HAVING COUNT(OrderID) > 1;