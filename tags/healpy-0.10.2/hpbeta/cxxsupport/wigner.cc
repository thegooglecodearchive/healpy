/*
 *  This file is part of libcxxsupport.
 *
 *  libcxxsupport is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libcxxsupport is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libcxxsupport; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 *  libcxxsupport is being developed at the Max-Planck-Institut fuer Astrophysik
 *  and financially supported by the Deutsches Zentrum fuer Luft- und Raumfahrt
 *  (DLR).
 */

/*! \file wigner.cc
 *  Several C++ classes for calculating Wigner matrices
 *
 *  Copyright (C) 2009,2010 Max-Planck-Society
 *  \author Martin Reinecke and others (see individual classes)
 */

#include "wigner.h"
#include "lsconstants.h"

using namespace std;

void wigner_d_halfpi_risbo_scalar::do_line0 (double *l1, int j)
  {
  double xj = pq/j;
  for (int i=n; i>=1; --i)
    l1[i] = xj*sqt[j]*(sqt[j-i]*l1[i] - sqt[i]*l1[i-1]);
  l1[0] = pq*l1[0];
  }
void wigner_d_halfpi_risbo_scalar::do_line (const double *l1, double *l2,
  int j, int k)
  {
  double xj = pq/j;
  double t1 = xj*sqt[j-k];
  double t2 = xj*sqt[k];
  for (int i=n; i>=1; --i)
    l2[i] = t1 * (sqt[j-i]*l2[i] - sqt[i]*l2[i-1])
            +t2 * (sqt[j-i]*l1[i] + sqt[i]*l1[i-1]);
  l2[0] = sqt[j] * (t2*l1[0]+t1*l2[0]);
  }

wigner_d_halfpi_risbo_scalar::wigner_d_halfpi_risbo_scalar(int lmax)
  : pq(.5*sqrt(2.)), sqt(2*lmax+1), d(lmax+2,lmax+2), n(-1)
  { for (tsize m=0; m<sqt.size(); ++m) sqt[m] = sqrt(double(m)); }

const arr2<double> &wigner_d_halfpi_risbo_scalar::recurse ()
  {
  ++n;
  if (n==0)
    d[0][0] = 1;
  else if (n==1)
    {
    d[0][0] = .5; d[0][1] =-pq;
    d[1][0] = pq; d[1][1] = 0.;
    }
  else
    {
//padding
    int flip = 1;
    for (int i=0; i<n; ++i)
      {
      d[i][n]=flip*d[i][n-2];
      d[n][i]=flip*d[n-2][i];
      flip=-flip;
      }
    d[n][n]=flip*d[n-2][n];

    do_line (d[n-1],d[n],2*n-1,n);
    for (int k=n; k>=2; --k)
      {
      do_line (d[k-2],d[k-1],2*n-1,k-1);
      do_line (d[k-1],d[k],2*n,k);
      }
    do_line0 (d[0],2*n-1);
    do_line (d[0],d[1],2*n,1);
    do_line0 (d[0],2*n);
    }
  return d;
  }

void wigner_d_risbo_scalar::do_line0 (double *l1, int j)
  {
  double xj = 1./j;
  l1[j] = -p*l1[j-1];
  for (int i=j-1; i>=1; --i)
    l1[i] = xj*sqt[j]*(q*sqt[j-i]*l1[i] - p*sqt[i]*l1[i-1]);
  l1[0] = q*l1[0];
  }
void wigner_d_risbo_scalar::do_line (const double *l1, double *l2, int j, int k)
  {
  double xj = 1./j;
  double t1 = xj*sqt[j-k]*q, t2 = xj*sqt[j-k]*p;
  double t3 = xj*sqt[k]*p,   t4 = xj*sqt[k]*q;
  l2[j] = sqt[j] * (t4*l1[j-1]-t2*l2[j-1]);
  for (int i=j-1; i>=1; --i)
    l2[i] = t1*sqt[j-i]*l2[i] - t2*sqt[i]*l2[i-1]
            +t3*sqt[j-i]*l1[i] + t4*sqt[i]*l1[i-1];
  l2[0] = sqt[j] * (t3*l1[0]+t1*l2[0]);
  }

wigner_d_risbo_scalar::wigner_d_risbo_scalar(int lmax, double ang)
  : p(sin(ang/2)), q(cos(ang/2)), sqt(2*lmax+1),
    d(lmax+1,2*lmax+1), n(-1)
  { for (tsize m=0; m<sqt.size(); ++m) sqt[m] = sqrt(double(m)); }

const arr2<double> &wigner_d_risbo_scalar::recurse ()
  {
  ++n;
  if (n==0)
    d[0][0] = 1;
  else if (n==1)
    {
    d[0][0] = q*q; d[0][1] = -p*q*sqt[2]; d[0][2] = p*p;
    d[1][0] = -d[0][1]; d[1][1] = q*q-p*p; d[1][2] = d[0][1];
    }
  else
    {
    // padding
    int sign = (n&1)? -1 : 1;
    for (int i=0; i<=2*n-2; ++i)
      {
      d[n][i] = sign*d[n-2][2*n-2-i];
      sign=-sign;
      }
    do_line (d[n-1],d[n],2*n-1,n);
    for (int k=n; k>=2; --k)
      {
      do_line (d[k-2],d[k-1],2*n-1,k-1);
      do_line (d[k-1],d[k],2*n,k);
      }
    do_line0 (d[0],2*n-1);
    do_line (d[0],d[1],2*n,1);
    do_line0 (d[0],2*n);
    }
  return d;
  }

wigner_d_halfpi_risbo_openmp::wigner_d_halfpi_risbo_openmp(int lmax)
  : pq(.5*sqrt(2.)), sqt(2*lmax+1), d(lmax+2,lmax+2),
    dd(lmax+2,lmax+2), n(-1)
  { for (tsize m=0; m<sqt.size(); ++m) sqt[m] = sqrt(double(m)); }

const arr2<double> &wigner_d_halfpi_risbo_openmp::recurse ()
  {
  ++n;
  if (n==0)
    d[0][0] = 1;
  else if (n==1)
    {
    d.fast_alloc(3,3);
    d[0][0] = .5; d[0][1] =-pq;
    d[1][0] = pq; d[1][1] = 0.;
    }
  else
    {
//padding
    int flip = 1;
    for (int i=0; i<n; ++i)
      {
      d[i][n]=flip*d[i][n-2];
      d[n][i]=flip*d[n-2][i];
      flip=-flip;
      }
    d[n][n]=flip*d[n-2][n];

    for (int j=2*n-1; j<=2*n; ++j)
      {
      dd.fast_alloc(n+2,n+2);
      double tmpx1 = pq/j;
      dd[0][0] = pq*d[0][0];
      for (int i=1;i<=n; ++i)
        dd[0][i] = tmpx1*sqt[j]*(sqt[j-i]*d[0][i] - sqt[i]*d[0][i-1]);
#pragma omp parallel
{
      int k;
#pragma omp for schedule(static)
      for (k=1; k<=n; ++k)
        {
        double stmp1=sqt[j-k]*tmpx1;
        double stmp2=sqt[k]*tmpx1;
        double save1 = stmp1*d[k][0], save2 = stmp2*d[k-1][0];
        dd[k][0] = sqt[j]*(save1+save2);
        for (int i=1; i<=n; ++i)
          {
          dd[k][i] = sqt[i]*(save2-save1);
          save1 = stmp1*d[k][i];
          save2 = stmp2*d[k-1][i];
          dd[k][i] += sqt[j-i]*(save1+save2);
          }
        }
}
      dd.swap(d);
      }
    }
  return d;
  }

wigner_d_risbo_openmp::wigner_d_risbo_openmp(int lmax, double ang)
  : p(sin(ang/2)), q(cos(ang/2)), sqt(2*lmax+1),
    d(lmax+1,2*lmax+1), dd(lmax+1,2*lmax+1), n(-1)
  { for (tsize m=0; m<sqt.size(); ++m) sqt[m] = sqrt(double(m)); }

const arr2<double> &wigner_d_risbo_openmp::recurse ()
  {
  ++n;
  if (n==0)
    d[0][0] = 1;
  else if (n==1)
    {
    d[0][0] = q*q; d[0][1] = -p*q*sqt[2]; d[0][2] = p*p;
    d[1][0] = -d[0][1]; d[1][1] = q*q-p*p; d[1][2] = d[0][1];
    }
  else
    {
    // padding
    int sign = (n&1)? -1 : 1;
    for (int i=0; i<=2*n-2; ++i)
      {
      d[n][i] = sign*d[n-2][2*n-2-i];
      sign=-sign;
      }
    for (int j=2*n-1; j<=2*n; ++j)
      {
      double xj = 1./j;
      dd[0][0] = q*d[0][0];
      for (int i=1;i<j; ++i)
        dd[0][i] = xj*sqt[j]*(q*sqt[j-i]*d[0][i] - p*sqt[i]*d[0][i-1]);
      dd[0][j] = -p*d[0][j-1];
#pragma omp parallel
{
      int k;
#pragma omp for schedule(static)
      for (k=1; k<=n; ++k)
        {
        double t1 = xj*sqt[j-k]*q, t2 = xj*sqt[j-k]*p;
        double t3 = xj*sqt[k  ]*p, t4 = xj*sqt[k  ]*q;
        dd[k][0] = xj*sqt[j]*(q*sqt[j-k]*d[k][0] + p*sqt[k]*d[k-1][0]);
        for (int i=1; i<j; ++i)
          dd[k][i] = t1*sqt[j-i]*d[k  ][i] - t2*sqt[i]*d[k  ][i-1]
                    + t3*sqt[j-i]*d[k-1][i] + t4*sqt[i]*d[k-1][i-1];
        dd[k][j] = -t2*sqt[j]*d[k][j-1] + t4*sqt[j]*d[k-1][j-1];
        }
}
      dd.swap(d);
      }
    }
  return d;
  }


wignergen::wignergen (int lmax_, const arr<double> &thetas, double epsilon)
  : eps(epsilon), lmax(lmax_),
    logsum(2*lmax+1), lc05(thetas.size()), ls05(thetas.size()),
    flm1(2*lmax+1), flm2(2*lmax+1),
    cf(maxscale+1-minscale), costh(thetas.size()), xl(lmax+1),
    thetaflip(thetas.size()),
    m1(-1234567890), m2(-1234567890), am1(-1234567890), am2(-1234567890),
    mlo(-1234567890), mhi(-1234567890),
    fx(lmax+2), result(lmax+1)
#ifdef PLANCK_HAVE_SSE2
    , result2(lmax+1)
#endif
  {
  planck_assert(lmax>0,"lmax too small");
  logsum[0] = 0.;
  for (tsize m=1; m<logsum.size(); ++m)
    logsum[m] = logsum[m-1]+log(static_cast<long double>(m));
  for (tsize lm=0; lm<flm1.size(); ++lm)
    {
    flm1[lm] = sqrt(1./(lm+1.));
    flm2[lm] = sqrt(lm/(lm+1.));
    }
  for (tsize i=0; i<cf.size(); ++i)
    cf[i] = ldexp(1.,(int(i)+minscale)*large_exponent2);

  fsmall = ldexp(1.,-large_exponent2);
  fbig = ldexp(1.,large_exponent2);

  for (tsize i=0; i<thetas.size(); ++i)
    {
    double theta=fmodulo(thetas[i],twopi);
    if (theta>pi) theta-=twopi;
    thetaflip[i]=(theta<0);
    theta=abs(theta); // now theta is in (0; pi)
    // tiny adjustments to make sure cos and sin (theta/2) are positive
    if (theta==0.) theta=1e-16;
    if (abs_approx(theta,pi,1e-15)) theta=pi-1e-15;
    costh[i]=cos(theta);
    lc05[i]=log(cos(0.5L*theta));
    ls05[i]=log(sin(0.5L*theta));
    }
  xl[0]=0;
  for (tsize l=1; l<xl.size(); ++l) xl[l]=1./l;

  for (tsize l=0; l<fx.size(); ++l)
    fx[l][0]=fx[l][1]=fx[l][2]=0.;
  }

void wignergen::prepare (int m1_, int m2_)
  {
  if ((m1_==m1) && (m2_==m2)) return;

  int mlo_=abs(m1_), mhi_=abs(m2_);
  if (mhi_<mlo_) swap(mhi_,mlo_);
  bool ms_similar = ((mhi==mhi_) && (mlo==mlo_));
  bool flip_m_sign = ((m1*m2)!=(m1_*m2_));

  m1=m1_; m2=m2_;
  mlo=am1=abs(m1); mhi=am2=abs(m2);
  if (mhi<mlo) swap(mhi,mlo);

  if (ms_similar)
    {
    if (flip_m_sign)
      for (int l=mhi; l<lmax; ++l)
        fx[l+1][1]=-fx[l+1][1];
    }
  else
    {
    for (int l=mhi; l<lmax; ++l)
      {
      double t = flm1[l+m1]*flm1[l-m1]*flm1[l+m2]*flm1[l-m2];
      double lt = 2*l+1;
      double l1 = l+1;
      fx[l+1][0]=l1*lt*t;
      fx[l+1][1]=m1*m2*xl[l]*xl[l+1];
      t = flm2[l+m1]*flm2[l-m1]*flm2[l+m2]*flm2[l-m2];
      fx[l+1][2]=t*l1*xl[l];
      }
    }

  prefactor = 0.5L*(logsum[2*mhi]-logsum[mhi+mlo]-logsum[mhi-mlo]);

  preMinus = false;
  if (mhi==am1)
    {
    cosPow = mhi-m2; sinPow = mhi+m2;
    if (m1>=0)
      { swap(cosPow, sinPow); preMinus=((mhi-m2)&1); }
    }
  else
    {
    cosPow = mhi+m1; sinPow = mhi-m1;
    if (m2<0)
      { swap(cosPow, sinPow); preMinus=((mhi+m1)&1); }
    }
  }

const arr<double> &wignergen::calc (int nth, int &firstl)
  {
  int l=mhi;
  const dbl3 *fy = &fx[0];
  const double cth = costh[nth];
  double *res = &result[0];
  long double logval = prefactor + lc05[nth]*cosPow + ls05[nth]*sinPow;
  logval *= inv_ln2;
  int scale = int (logval/large_exponent2)-minscale;
  double rec1 = 0.;
  double rec2 = double(exp(ln2*(logval-(scale+minscale)*large_exponent2)));
  if (preMinus ^ (thetaflip[nth] && ((am1+am2)&1))) rec2 = -rec2;

  while(scale<0) // iterate until we reach the realm of IEEE numbers
    {
    if (++l>lmax) break;
    rec1 = (cth - fy[l][1])*fy[l][0]*rec2 - fy[l][2]*rec1;
    if (++l>lmax) break;
    rec2 = (cth - fy[l][1])*fy[l][0]*rec1 - fy[l][2]*rec2;

    while (abs(rec2)>fbig)
      {
      rec1 *= fsmall;
      rec2 *= fsmall;
      ++scale;
      }
    }

  if (scale<0) { firstl=lmax+1; return result; }
  rec1 *= cf[scale];
  rec2 *= cf[scale];

  for (;l<lmax-1;l+=2) // iterate until we cross the eps threshold
    {
    if (abs(rec2)>eps) break;
    rec1 = (cth - fy[l+1][1])*fy[l+1][0]*rec2 - fy[l+1][2]*rec1;
    if (abs(rec1)>eps) { swap(rec1,rec2); ++l; break; }
    rec2 = (cth - fy[l+2][1])*fy[l+2][0]*rec1 - fy[l+2][2]*rec2;
    }
  if ((abs(rec2)<=eps) && (++l<=lmax))
    {
    rec1 = (cth - fy[l][1])*fy[l][0]*rec2 - fy[l][2]*rec1;
    swap (rec1,rec2);
    }

  firstl = l;
  if (l>lmax) return result;

  res[l]=rec2;

  for (;l<lmax-1;l+=2)
    {
    res[l+1] = rec1 = (cth - fy[l+1][1])*fy[l+1][0]*rec2 - fy[l+1][2]*rec1;
    res[l+2] = rec2 = (cth - fy[l+2][1])*fy[l+2][0]*rec1 - fy[l+2][2]*rec2;
    }
  while (true)
    {
    if (++l>lmax) break;
    res[l] = rec1 = (cth - fy[l][1])*fy[l][0]*rec2 - fy[l][2]*rec1;
    if (++l>lmax) break;
    res[l] = rec2 = (cth - fy[l][1])*fy[l][0]*rec1 - fy[l][2]*rec2;
    }

  return result;
  }

#ifdef PLANCK_HAVE_SSE2

#define RENORMALIZE \
  do \
    { \
    double rec1a, rec1b, rec2a, rec2b, cfa, cfb; \
    read_v2df (rec1, &rec1a, &rec1b); read_v2df (rec2, &rec2a, &rec2b); \
    read_v2df (corfac, &cfa, &cfb); \
    while (abs(rec2a)>fbig) \
      { \
      rec1a*=fsmall; rec2a*=fsmall; ++scale1; \
      cfa = (scale1<0) ? 0. : cf[scale1]; \
      } \
    while (abs(rec2b)>fbig) \
      { \
      rec1b*=fsmall; rec2b*=fsmall; ++scale2; \
      cfb = (scale2<0) ? 0. : cf[scale2]; \
      } \
    rec1=build_v2df(rec1a,rec1b); rec2=build_v2df(rec2a,rec2b); \
    corfac=build_v2df(cfa,cfb); \
    } \
  while(0)

#define GETPRE(prea,preb,lv) \
  prea=_mm_mul_pd(_mm_sub_pd(cth,_mm_set1_pd(fy[lv][1])),_mm_set1_pd(fy[lv][0])); \
  preb=_mm_set1_pd(fy[lv][2]);

#define NEXTSTEP(prea,preb,prec,pred,reca,recb,lv) \
  { \
  prec = _mm_set1_pd(fy[lv][1]); \
  preb = _mm_mul_pd(preb,reca); \
  prea = _mm_mul_pd(prea,recb); \
  v2df t0 = _mm_set1_pd(fy[lv][0]); \
  prec = _mm_sub_pd(cth,prec); \
  pred = _mm_set1_pd(fy[lv][2]); \
  reca = _mm_sub_pd(prea,preb); \
  prec = _mm_mul_pd(prec,t0); \
  }

const arr_align<v2df,16> &wignergen::calc (int nth1, int nth2, int &firstl)
  {
  int l=mhi;
  const dbl3 *fy = &fx[0];
  const v2df cth = build_v2df(costh[nth1],costh[nth2]);
  v2df *res = &result2[0];
  long double logval1 = prefactor + lc05[nth1]*cosPow + ls05[nth1]*sinPow,
              logval2 = prefactor + lc05[nth2]*cosPow + ls05[nth2]*sinPow;
  logval1 *= inv_ln2;
  logval2 *= inv_ln2;
  int scale1 = int (logval1/large_exponent2)-minscale,
      scale2 = int (logval2/large_exponent2)-minscale;
  v2df rec1 = _mm_setzero_pd();
  double tr1 = double(exp(ln2*(logval1-(scale1+minscale)*large_exponent2))),
         tr2 = double(exp(ln2*(logval2-(scale2+minscale)*large_exponent2)));
  if (preMinus ^ (thetaflip[nth1] && ((am1+am2)&1))) tr1 = -tr1;
  if (preMinus ^ (thetaflip[nth2] && ((am1+am2)&1))) tr2 = -tr2;
  v2df rec2 = build_v2df(tr1,tr2);
  v2df corfac = build_v2df ( (scale1<0) ? 0. : cf[scale1],
                             (scale2<0) ? 0. : cf[scale2]);

  v2df eps2=build_v2df(eps,eps);
  v2df fbig2=build_v2df(fbig,fbig);

  v2df pre0,pre1,pre2,pre3;

  GETPRE(pre0,pre1,l+1)
  if ((scale1<0) && (scale2<0))
    {
    while (true)
      {
      if (++l>lmax) break;
      NEXTSTEP(pre0,pre1,pre2,pre3,rec1,rec2,l+1)
      if (++l>lmax) break;
      NEXTSTEP(pre2,pre3,pre0,pre1,rec2,rec1,l+1)
      if (v2df_any_gt(rec2,fbig2))
        {
        RENORMALIZE;
        if ((scale1>=0) || (scale2>=0)) break;
        }
      }
    }

  if (l<=lmax)
    {
    GETPRE(pre0,pre1,l+1)
    while (true)
      {
      v2df t1;
      res[l]=t1=_mm_mul_pd(rec2,corfac);
      if (v2df_any_gt(t1,eps2))
        break;

      if (++l>lmax) break;
      NEXTSTEP(pre0,pre1,pre2,pre3,rec1,rec2,l+1)

      res[l]=t1=_mm_mul_pd(rec1,corfac);
      if (v2df_any_gt(t1,eps2))
        { swap(rec1,rec2); break; }

      if (++l>lmax) break;
      NEXTSTEP(pre2,pre3,pre0,pre1,rec2,rec1,l+1)

      if (v2df_any_gt(rec2,fbig2))
        RENORMALIZE;
      }
    }
  firstl=l;
  if (l>lmax) return result2;

  GETPRE(pre0,pre1,l+1)
  while (true)
    {
    v2df t1;
    res[l]=t1=_mm_mul_pd(rec2,corfac);
    if (v2df_all_ge(t1,eps2))
      break;

    if (++l>lmax) break;
    NEXTSTEP(pre0,pre1,pre2,pre3,rec1,rec2,l+1)

    res[l]=t1=_mm_mul_pd(rec1,corfac);
    if (v2df_all_ge(t1,eps2))
      { swap(rec1,rec2); break; }

    if (++l>lmax) break;
    NEXTSTEP(pre2,pre3,pre0,pre1,rec2,rec1,l+1)

    if (v2df_any_gt(rec2,fbig2))
      RENORMALIZE;
    }

  if (l>lmax) return result2;
  rec1 = _mm_mul_pd (rec1,corfac);
  rec2 = _mm_mul_pd (rec2,corfac);

  GETPRE(pre0,pre1,l+1)
  for (;l<lmax-1;l+=2)
    {
    res[l] = rec2;
    NEXTSTEP(pre0,pre1,pre2,pre3,rec1,rec2,l+2)
    res[l+1] = rec1;
    NEXTSTEP(pre2,pre3,pre0,pre1,rec2,rec1,l+3)
    }

  res[l] = rec2;
  if (++l<=lmax)
    {
    NEXTSTEP(pre0,pre1,pre2,pre3,rec1,rec2,l+1)
    res[l] = rec1;
    }

  return result2;
  }

#endif /* PLANCK_HAVE_SSE2 */
