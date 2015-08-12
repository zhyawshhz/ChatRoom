#ifndef No1_Thread_Thread_h
#define No1_Thread_Thread_h

class No1Thread : public No1LogBaseClass
{
public:
	No1Thread();
	~No1Thread();

private:
	static void* threadFunc(void* args);
	void join();
	void cancel();
private:
	pthreat_t			mThrdId;
};

#endif
