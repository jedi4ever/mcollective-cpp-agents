#ifndef __MCOLLECTIVELISTENER__
#define __MCOLLECTIVELISTENER__

class McollectiveListener
{
  public:
    McollectiveListener();

    /* use virtual otherwise linker will try to perform static linkage */
    virtual void DoSomething();

  private:
    int x;
};

#endif
