double p[10];
int nP;
float x[1000 * nSlotNum];
float dX;

int jacobianInverse(int nP)
{
  p[nP] = abs(nP * (x[0] + dX) - p[1]);
  if(zAxis.read() < 150)
  {
    if(zAxis.read() > 30)
    {
      // in-range
      grabThebox();
    }
  }
}