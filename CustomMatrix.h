#ifndef CUSTOMMATRIX_H
#define CUSTOMMATRIX_H

#include <iostream>

using namespace std;

class CustomMatrix{
public:
    bool mat[3][3]={0};
    CustomMatrix(){

    }

    // Corrected to use fixed size 3, fixing the sizeof warning/bug
    CustomMatrix(bool m[3][3]){
        // const int m_size = sizeof(m) / sizeof(m[0]); // REMOVED
        for (int i=0;i< 3;i++) {
            for(int j=0; j < 3;j++){
                //cout << i << " " << j << endl;
                mat[i][j] = m[i][j];
            }
        }
    }

    // Corrected to use fixed size 3, fixing the sizeof warning/bug
    void fillMatrix(bool m[3][3]){
        // const int m_size = sizeof(m) / sizeof(m[0]); // REMOVED
        for (int i=0;i< 3;i++) {
            for(int j=0; j < 3;j++){
                //cout << i << " " << j << endl;
                mat[i][j] = m[i][j];
            }
        }
    }
};

#endif // CUSTOMMATRIX_H
