В программу echo.c добавить 1 поток (всего будет 2 потока, включая 1 главный поток).
В потоке должен быть свой event loop. Перенести TCP сокет в этот поток.
Приняв пакет, нужно переслать его на обработку в главный поток.
Главный поток должен что-то сделать с пакетом, например перевернуть все данные в нем задом наперед.
После этого пакет передается обратно в поток и там отсылается обратно клиенту. </br>
Компилирование: gcc -o ohce ohce.c -lev -pthread</br>
Вызов программы с переданным аргументом: ./ohce "номер порта"</br>
Тестирование: telnet 127.0.0.1 "номер порта"</br>
