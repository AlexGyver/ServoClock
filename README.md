![PROJECT_PHOTO](https://github.com/AlexGyver/ServoClock/blob/master/proj_img.jpg)
# Сервоприводные часы на Arduino
* [Описание проекта](#chapter-0)
* [Папки проекта](#chapter-1)
* [Схемы подключения](#chapter-2)
* [Материалы и компоненты](#chapter-3)
* [Как скачать и прошить](#chapter-4)
* [FAQ](#chapter-5)
* [Полезная информация](#chapter-6)
[![AlexGyver YouTube](http://alexgyver.ru/git_banner.jpg)](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)

<a id="chapter-0"></a>
## Описание проекта
Сервоприводные 7-сегментные часы на Arduino
Особенности:
- Хардкорный проект, состоящий из 28 сервоприводов и напечатанных на 3D принтере элементов
- Оборудованы датчиком движения, который позволяет не переключать время, пока людей рядом с часами нет, или они не шевелятся =)
- Проект использует RTC DS3231
- Подробности в видео: https://youtu.be/RL3j-3fyWM8
<a id="chapter-1"></a>
## Папки
**ВНИМАНИЕ! Если это твой первый опыт работы с Arduino, читай [инструкцию](#chapter-4)**
- **libraries** - библиотеки проекта. Заменить имеющиеся версии
- **servo-clock** - прошивка
- **images** - исходники картинок
- **schemes** - схемы подключения
- **models** - 3D модели и чертежи

<a id="chapter-2"></a>
## Схемы
![SCHEME](https://github.com/AlexGyver/ServoClock/blob/master/schemes/scheme1.jpg)
![SCHEME](https://github.com/AlexGyver/ServoClock/blob/master/schemes/scheme2.jpg)
![SCHEME](https://github.com/AlexGyver/ServoClock/blob/master/schemes/scheme3.jpg)

<a id="chapter-3"></a>
## Материалы и компоненты
### Ссылки оставлены на магазины, с которых я закупаюсь уже не один год
### Почти все компоненты можно взять в магазине WAVGAT по первым ссылкам
* Arduino NANO 328p – искать
* https://ali.ski/tI7blh
* https://ali.ski/O4yTxb
* https://ali.ski/6_rFIS
* https://ali.ski/gb92E-
* RTC DS3231
* https://ali.ski/lqQM2d
* https://ali.ski/u6mZas
* Датчик движения искать
* https://ali.ski/XjNWx
* Серво драйвер х16
* https://ali.ski/Mwi2ap
* https://ali.ski/T26inS
* Сервы (берите 30шт)
* https://ali.ski/LrykyU
* https://ali.ski/3wzSB
* https://ali.ski/b3pJ0
* БП 5V Али искать
* https://ali.ski/DItEG
* https://ali.ski/t3YFfU
* 3D печать на заказ от подписчиков канала https://alexgyver.ru/3d-print/
* Подборка недорогих 3D принтеров с обзорами https://alexgyver.ru/3d-printers/

## Вам скорее всего пригодится
* [Всё для пайки (паяльники и примочки)](http://alexgyver.ru/all-for-soldering/)
* [Недорогие инструменты](http://alexgyver.ru/my_instruments/)
* [Все существующие модули и сенсоры Arduino](http://alexgyver.ru/arduino_shop/)
* [Электронные компоненты](http://alexgyver.ru/electronics/)
* [Аккумуляторы и зарядные модули](http://alexgyver.ru/18650/)

<a id="chapter-4"></a>
## Как скачать и прошить
* [Первые шаги с Arduino](http://alexgyver.ru/arduino-first/) - ультра подробная статья по началу работы с Ардуино, ознакомиться первым делом!
* Скачать архив с проектом
> На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**
* Установить библиотеки в  
`C:\Program Files (x86)\Arduino\libraries\` (Windows x64)  
`C:\Program Files\Arduino\libraries\` (Windows x86)
* **Подключить внешнее питание 5 Вольт**
* Подключить Ардуино к компьютеру
* Запустить файл прошивки (который имеет расширение .ino)
* Настроить IDE (COM порт, модель Arduino, как в статье выше)
* Настроить что нужно по проекту
* Нажать загрузить
* Пользоваться  

## Настройки в коде
    #define SERVOMIN  150     // минимальный сигнал серво
    #define SERVOMAX  550     // максимальный сигнал серво
    #define IR_PIN 2          // пин ИК датчика
    #define IR_SENSOR 1       // использовать ИК датчик движения
    #define IR_TIMEOUT 300000 // таймаут датчика движения (умолч. 300000мс это 5 мин)
    #define STOCK_UP 270      // значение максимального угла (сегмент поднят) по умолчанию
    #define STRING_SPEED 500  // скорость бегущей строки
    // текст бегущей строки настраивается примерно в строке №100
	
<a id="chapter-5"></a>
## FAQ
### Основные вопросы
В: Как скачать с этого грёбаного сайта?  
О: На главной странице проекта (где ты читаешь этот текст) вверху справа зелёная кнопка **Clone or download**, вот её жми, там будет **Download ZIP**

В: Скачался какой то файл .zip, куда его теперь?  
О: Это архив. Можно открыть стандартными средствами Windows, но думаю у всех на компьютере установлен WinRAR, архив нужно правой кнопкой и извлечь.

В: Я совсем новичок! Что мне делать с Ардуиной, где взять все программы?  
О: Читай и смотри видос http://alexgyver.ru/arduino-first/

В: Вылетает ошибка загрузки / компиляции!
О: Читай тут: https://alexgyver.ru/arduino-first/#step-5

В: Сколько стоит?  
О: Ничего не продаю.

### Вопросы по этому проекту

<a id="chapter-6"></a>
## Полезная информация
* [Мой сайт](http://alexgyver.ru/)
* [Основной YouTube канал](https://www.youtube.com/channel/UCgtAOyEQdAyjvm9ATCi_Aig?sub_confirmation=1)
* [YouTube канал про Arduino](https://www.youtube.com/channel/UC4axiS76D784-ofoTdo5zOA?sub_confirmation=1)
* [Мои видеоуроки по пайке](https://www.youtube.com/playlist?list=PLOT_HeyBraBuMIwfSYu7kCKXxQGsUKcqR)
* [Мои видеоуроки по Arduino](http://alexgyver.ru/arduino_lessons/)