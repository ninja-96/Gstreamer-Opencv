# Gstreamer + OpenCV (С++)

Пайплайн на Gstreamer и получение изображение в формате Mat Opencv

# Зависимости
- Gstreamer 1.14 и выше
- OpenCV 4 и выше

## Сборка проекта
```
mkdir build && cd build
cmake ..
make -j8
```

## Запуск
```
./gstreamer_cxx <путь к фидеофайлу> <ширина выходного изображения> <высота выходного изображения>
```

# Описание 
GstVideoCap - класс, который показывает видеро из файла и изменяет разрешение кадра (на стороне Gstreamer)
```C
auto cap = GstVideoCap(std::string(argv[1]), atoi(argv[2]), atoi(argv[3]));
```
Первый параметр отвечает за путь к видео, второй и третий - ширина и высота изображения на выходе.

В классе метод **callback**, который симулирует полезную работу класса (обработка кадра после выполнения пайплайна).

В результате выполнения в консоль будет выводится информация следующего типа:
```
Callback
360 x 640
Callback
360 x 640
```
*Callback* - Просто вывод, показывающий что пайплайн отработал нормально\
*360 x 640* - Разрешение изображения OpenCV