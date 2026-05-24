#include "mainwindow.h"

#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(R"(
    /* ========== БАЗА ========== */
    QMainWindow {
        background-color: #f3f5f7; /* Немного холодный серый фон, глазу приятнее чистого белого */
    }

    /* ========== КНОПКИ (Премиум) ========== */
    QPushButton {
        background-color: #ffffff;
        color: #1f2937;
        border: 1px solid #e2e8f0;
        border-radius: 6px; /* Умеренное скругление */
        padding: 6px 14px;
        font-weight: 500;
        font-size: 12px;
    }
    QPushButton:hover {
        background-color: #f8fafc;
        border-color: #cbd5e1;
    }
    QPushButton:pressed {
        background-color: #f1f5f9;
    }
    /* Активная кнопка (синий акцент) */
    QPushButton:checked {
        background-color: #0078d4;
        color: white;
        border-color: #0078d4;
    }

    /* ========== ПАНЕЛЬ (ToolBar) ========== */
    QToolBar {
        background-color: #ffffff;
        border-bottom: 1px solid #e2e8f0;
        spacing: 8px;
        padding: 6px 12px;
    }
    QToolButton {
        background-color: transparent;
        border-radius: 6px;
        padding: 6px;
    }
    QToolButton:hover {
        background-color: #f1f5f9;
    }

    /* ========== ГРУППЫ (GroupBox) ========== */
    QGroupBox {
        color: #1e293b;
        font-weight: 600;
        border: 1px solid #e2e8f0;
        border-radius: 8px;
        margin-top: 12px;
        padding-top: 10px;
        background-color: #ffffff;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        left: 16px;
        padding: 0 8px;
        color: #64748b;
    }

    /* ========== СПИСОК ИЗМЕРЕНИЙ ========== */
    QListWidget {
        background-color: #ffffff;
        color: #334155;
        border: 1px solid #e2e8f0;
        border-radius: 8px;
        outline: none;
        padding: 4px;
    }
    QListWidget::item {
        padding: 8px 12px;
        border-radius: 6px;
        margin: 1px;
    }
    QListWidget::item:hover {
        background-color: #f8fafc;
    }
    QListWidget::item:selected {
        background-color: #eef2ff;  /* Легкая фиолетовая/синяя подсветка */
        color: #1e40af;
        font-weight: 500;
    }

    /* ========== ОБЛАСТЬ С КАРТИНКОЙ ========== */
    QGraphicsView {
        background-color: #f1f5f9;
        border: 1px solid #e2e8f0;
        border-radius: 12px;
        /* Добавим легкую тень для глубины */
    }

    /* ========== МЕНЮ ========== */
    QMenuBar {
        background-color: #ffffff;
        color: #334155;
        border-bottom: 1px solid #e2e8f0;
    }
    QMenuBar::item:selected {
        background-color: #f1f5f9;
    }
    QMenu {
        background-color: #ffffff;
        border: 1px solid #e2e8f0;
        border-radius: 8px;
        padding: 4px;
    }
    QMenu::item:selected {
        background-color: #f1f5f9;
        color: #0078d4;
    }

    /* ========== СТАТУС БАР ========== */
    QStatusBar {
        color: #64748b;
        border-top: 1px solid #e2e8f0;
        padding: 2px 8px;
        font-size: 11px;
    }
)");
    MainWindow w;
    w.show();
    return a.exec();
}
