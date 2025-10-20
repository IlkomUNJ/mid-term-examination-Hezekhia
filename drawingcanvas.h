#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <iostream>
#include <iomanip>
#include <QPixmap>
#include <fstream> // <-- ADDED: Needed for std::ofstream
#include <string>  // <-- ADDED: Needed for string operations (in dumpWindow)
#include "CustomMatrix.h"

using namespace std;
using Array3x3 = std::array<std::array<bool, 3>, 3>;


class DrawingCanvas : public QWidget
{
    Q_OBJECT
private:
    const int WINDOW_WIDTH=600;
    const int WINDOW_HEIGHT=400;

public:
    explicit DrawingCanvas(QWidget *parent = nullptr);

    // Slot to clear all points from the canvas
    void clearPoints();
    void paintLines();
    void segmentDetection();

protected:
    // Overridden method to handle painting on the widget
    void paintEvent(QPaintEvent *event) override;

    // Overridden method to handle mouse clicks
    void mousePressEvent(QMouseEvent *event) override;

private:
    // --- ADDED: Declarations for the helper functions ---
    // Note: We use the C-style array syntax bool[3][3] for consistency with the .cpp definition
    bool isNonEmpty(const bool local_window[3][3], double& score);
    void dumpWindow(std::ofstream& logFile, int x, int y, const bool local_window[3][3], double score);

    // A vector to store all the points drawn by the user
    QVector<QPoint> m_points;

    bool isPaintLinesClicked = false;
};
#endif // DRAWINGCANVAS_H
