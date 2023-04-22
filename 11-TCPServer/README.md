# 11: TCP Server 

Разработать клиент-серверное приложение, использующее TCP/IP и реализующее следующие взаимодействия: клиент №1 передает сообщения серверу, который перенаправляет их клиенту №2.   
В качестве исходных данных задавать: IP-адрес сервера, порт сервера. Эти данные можно задавать либо в командной строке, либо с использованием функций стандартного ввода.  
  
Передаваемое сообщение задается из стандартного ввода в цикле. То есть, можно передать не одно а несколько сообщений от клиента №1 клиенту №2.   
При передаче сообщения "The End" необходимо завершить работу как клиентов, так и сервера.  

Приложение должно функционировать в конфигурациях от одного до трех компьютеров с различными комбинациями размещения клиентов и сервера.  

- `./src/`: исходники программ на `C`;
- `$ make` для сборки.

Запускайте программы в порядке `server` -> `client` -> `client`.
Так как функционал первого и второго клиента отличается (первый отправляет сообщения серверу - передатчик, а второй принимает сообщения от сервера и выводит на экран - получатель), то сервер сначала задает поведение клиента отправляя им байты `S` для передатчика и `R` для получателя.    