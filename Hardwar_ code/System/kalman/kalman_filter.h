#ifndef  _KALMAN_FILTER_H
#define  _KALMAN_FILTER_H

/* 
 * NOTES: n Dimension means the state is n dimension, 
 * measurement always 1 dimension 
 */

/* 1 Dimension */
typedef struct {
    float x;  /* state */
    float A;  /* x(n)=A*x(n-1)+u(n),u(n)~N(0,q) */
    float H;  /* z(n)=H*x(n)+w(n),w(n)~N(0,r)   */
    float q;  /* process(predict) noise convariance */
    float r;  /* measure noise convariance */
    float p;  /* estimated error convariance */
    float gain;
} kalman;

extern void kalman_init(kalman *state, float init_x, float init_p);
extern float kalman_filter(kalman *state, float z_measure);

#endif  /*_KALMAN_FILTER_H*/

