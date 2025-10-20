#include "drawingcanvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QTextStream>
#include <fstream>
#include <string>

// --- Constants ---
const QString LOG_FILENAME = "analysis_dump_3x3.csv"; // Dump file for assessment
const int WINDOW_SIZE = 3; // The current fixed window size in the provided code

// --- Helper function declarations (Private methods in DrawingCanvas) ---
// Note: These should be declared in drawingcanvas.h
bool isNonEmpty(const bool local_window[WINDOW_SIZE][WINDOW_SIZE], double& score);
void dumpWindow(std::ofstream& logFile, int x, int y, const bool local_window[WINDOW_SIZE][WINDOW_SIZE], double score);

// --- DrawingCanvas Methods ---

DrawingCanvas::DrawingCanvas(QWidget *parent) : QWidget(parent) {
    // Set a minimum size for the canvas
    setMinimumSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    // Set a solid background color
    setStyleSheet("background-color: white; border: 1px solid gray;");
}

void DrawingCanvas::clearPoints(){
    m_points.clear();
    // Trigger a repaint to clear the canvas
    update();
}

void DrawingCanvas::paintLines(){
    /* Todo
     * Implement lines drawing per even pair
     */
    isPaintLinesClicked = true;
    update();
}

/**
 * @brief Assesses a 3x3 boolean window for relevance ("non-empty") and calculates a score.
 * @param local_window The 3x3 array of true/false values (true if pixel is NOT white/background).
 * @param score Output parameter for the confidence score.
 * @return true if the window is considered "non-empty" and worth logging, false otherwise.
 */
bool DrawingCanvas::isNonEmpty(const bool local_window[WINDOW_SIZE][WINDOW_SIZE], double& score) {
    int count = 0;
    for (int m = 0; m < WINDOW_SIZE; ++m) {
        for (int n = 0; n < WINDOW_SIZE; ++n) {
            if (local_window[m][n]) {
                count++;
            }
        }
    }

    // --- Goal 1: Check for "Non-Empty" ---
    // A simple definition: If it contains at least 2 non-background pixels.
    if (count < 2) {
        score = 0.0;
        return false;
    }

    // --- Goal 2: Initial Assessment Logic (Pattern Fitting) ---
    // Example Assessment: Calculate density as the initial score.
    score = (double)count / (WINDOW_SIZE * WINDOW_SIZE);

    // You should refine this logic based on expected segment features (e.g., is it a corner, an edge, etc.)
    // For now, any window with a density >= 0.25 is considered a candidate for the dump.
    return score >= 0.25;
}


/**
 * @brief Dumps the candidate window information to the log file.
 */
void DrawingCanvas::dumpWindow(std::ofstream& logFile, int x, int y, const bool local_window[WINDOW_SIZE][WINDOW_SIZE], double score) {
    // Format the 3x3 boolean pattern as a string (e.g., "101010101")
    std::string patternString = "";
    for (int m = 0; m < WINDOW_SIZE; ++m) {
        for (int n = 0; n < WINDOW_SIZE; ++n) {
            patternString += (local_window[m][n] ? '1' : '0');
        }
    }

    // Write to the log file in CSV format: Size, X, Y, Score, PatternContent
    logFile << WINDOW_SIZE << "x" << WINDOW_SIZE << ","
            << x << ","
            << y << ","
            << score << ","
            << patternString << "\n";
}


void DrawingCanvas::segmentDetection(){
    QPixmap pixmap = this->grab();
    QImage image = pixmap.toImage();

    cout << "image width " << image.width() << endl;
    cout << "image height " << image.height() << endl;

    // --- 1. Open Log File for Dumping ---
    std::ofstream logFile(LOG_FILENAME.toStdString());
    if (!logFile.is_open()) {
        qDebug() << "Error opening log file:" << LOG_FILENAME;
        return;
    }
    logFile << "Size,X,Y,Score,PatternContent\n"; // CSV Header
    qDebug() << "Dumping 3x3 windows to:" << LOG_FILENAME;


    //To not crash we set initial size of the matrix
    // Note: The vector<CustomMatrix> windows is currently not used for dumping,
    // but we leave it as per your original code.
    vector<CustomMatrix> windows(image.width()*image.height());

    // Get the pixel value as an ARGB integer (QRgb is a typedef for unsigned int)
    // The loops (i, j) implement the sliding window for the 3x3 kernel.
    // i: row (y-coord), j: column (x-coord)
    for(int i = 1; i < image.height()-1; i++){ // Starts at 1, ends before image.height()-1 (Y-axis)
        for(int j = 1; j < image.width()-1; j++){ // Starts at 1, ends before image.width()-1 (X-axis)
            bool local_window[WINDOW_SIZE][WINDOW_SIZE] = {false};

            // This inner loop extracts the 3x3 window around (j, i)
            for(int m = -1; m <= 1; m++){ // m: row offset
                for(int n = -1; n <= 1; n++){ // n: column offset
                    QRgb rgbValue = image.pixel(j + n, i + m);
                    // The 'segment' is defined by any non-white pixel (0xffffffff is white)
                    local_window[m + 1][n + 1] = (rgbValue != 0xffffffff);
                }
            }

            // --- 2. Assessment Logic ---
            double score = 0.0;
            if (isNonEmpty(local_window, score)) {
                // --- 3. Dumping the Non-Empty Window ---
                dumpWindow(logFile, j, i, local_window, score);
            }

            // Original code to collect CustomMatrix (kept for compatibility)
            CustomMatrix mat(local_window);
            windows.push_back(mat);
        }
    }

    logFile.close();
    qDebug() << "Window dumping complete. Analyze " << LOG_FILENAME << " for ideal size/pattern.";
    return;
}

void DrawingCanvas::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set up the pen and brush for drawing the points
    QPen pen(Qt::blue, 5);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::blue));

    // Draw a small circle at each stored point
    for (const QPoint& point : std::as_const(m_points)) {
        painter.drawEllipse(point, 3, 3);
    }

    if(isPaintLinesClicked){
        cout << "paint lines block is called" << endl;
        pen.setColor(Qt::red);
        pen.setWidth(4); // 4-pixel wide line
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);

        // Set the painter's pen to our custom pen.
        painter.setPen(pen);

        for(int i=0;i<m_points.size()-1;i+=2){
            //cout << m_points[i].x() << endl;
            painter.drawLine(m_points[i], m_points[i+1]);
        }
        isPaintLinesClicked = false;

        //return painter pen to blue
        pen.setColor(Qt::blue);
        painter.setPen(pen);
    }
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    // Add the mouse click position to our vector of points
    m_points.append(event->pos());
    // Trigger a repaint
    update();
}
