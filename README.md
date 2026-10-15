# UMAS GUI

Операторский пульт управления подводными аппаратами. Qt-приложение получает телеметрию и видео через ROS 2, отображает состояние аппарата и отправляет управляющие команды.

![Пример интерфейса UMAS GUI](resources/images/gui.png)

## Документация

- [Сборка и запуск](docs/build-and-run.md)
- [Архитектура и структура проекта](docs/architecture.md)
- [Правила разработки](docs/contributing.md)
- [Стиль кода](docs/coding-style.md)
- [Содержание документации](docs/README.md)

Основная разработка ведётся в отдельных ветках с последующим Pull Request в
`umas-gui_2.0`.

Первоначальная настройка, сборка и создание ярлыка:

```bash
./scripts/install-cli.sh
umas-gui docker-build
umas-gui build
./scripts/install-desktop-shortcut.sh
```

После этого приложение можно запускать ярлыком `UMAS GUI` или командой
`umas-gui run`.
