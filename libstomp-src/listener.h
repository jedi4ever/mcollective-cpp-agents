#ifndef __MCOLLECTIVELISTENER__
#define __MCOLLECTIVELISTENER__


namespace Mcollective {
class Listener
{
  public:
   Listener();

    /* use virtual otherwise linker will try to perform static linkage */
    virtual void StartThreads();

    //virtual void onException(const CMSException);

  private:
    int x;
};
}

#endif
