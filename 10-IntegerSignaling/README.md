# 10: Integer Signaling
Побитовая передача целого числа от одного процесса другому с помощью сигналов `SIGUSR1`, `SIGUSR2`.  
  
Необходимо написать две программы — передатчик и приемник, которые будут запускаться с разных терминалов.   
Каждая программа вначале печатает свой PID и запрашивает ввод с клавиатуры PID программы–собеседника (вводится пользователем).  
Порядок ввода PID не имеет значения.   
Программа-передатчик запрашивает с клавиатуры ввод целого десятичного числа (число может быть и отрицательным!).   
Введенное число побитово передается программе-приемнику с использованием пользовательских сигналов `SIGUSR1` и `SIGUSR2`.   
Программа–приемник после завершения приема печатает принятое число в десятичной системе счисления.  
  
Примечание: Каждый бит (0 или 1) передается своим сигналом.   
Из-за ненадежности сигналов передача последовательности одинаковых битов может приводить к ошибкам.  
Поэтому каждый новый бит можно передавать только после подтверждения от приемника принятия предыдущего с помощью одного из пользовательских сигналов.   
Отсутствие подтверждения – грубая ошибка! Завершение передачи можно осуществить, например, передачей приемнику сигнала `SIGINT`.  

- `./src/`: исходники программ на `C`;
- `$ make` для сборки.

Для передачи бит используются сигналы `SIGNAL_BIT_0` и `SIGNAL_BIT_1`, означающие передачу 0 и 1 соответственно. Значения для данных констант контролируются `#define SIGNAL_BIT_0 SIGUSR1` и `#define SIGNAL_BIT_1 SIGUSR2`.  
После прочтения сигнала (обработки бита), приемник посылает сигнал `SIGNAL_BIT_1` передатчику в качестве подтверждения. Передатчик ждет подтверждения, прежде чем передать следующий бит.
Число передается в порядке от старшего бита к младшему (big endian).  
Передача заканчивается отправкой сигнала `SIGINT` приемнику.  
Программа не проверяет на корректность PID и целого числа, введенных пользователем.  

- `sender`: побитово передает введенное число пользователем приемнику;
- `receiver`: побитово получает число от передатчика и выводит его. 
