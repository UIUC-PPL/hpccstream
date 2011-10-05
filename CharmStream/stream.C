#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>


using namespace std;

#include "stream.decl.h"

#define N 4000000

CProxy_Controller controllerProxy;
int n;
double scalar;

class Controller:public CBase_Controller
{
		private:
				int count;
				int numIterations;
				int numOfPhasesDone; 
				CProxy_Worker workerProxy;
				double startTime, stopTime;
		public:
				Controller(CkArgMsg *msg)
						{
							
								workerProxy = CProxy_Worker::ckNew();
								numOfPhasesDone = 0;
								count =0;
								numIterations = 10;
								scalar = 3.0;
								controllerProxy = thisProxy;

							//	CkCallback * cb = new CkCallback(CkIndex_Controller::ReductionCallback(NULL), thisProxy);
							//	cells.ckSetReductionClient(cb);

								startTime = CmiWallTimer();
								workerProxy.Copy();

							//	ckout<<"Main done"<<endl;


						}
				Controller(CkMigrateMessage * msg){}
				/*void Done()
				{

						count++;
						if(count == numOfChares)
						{
								stopTime = CmiWallTimer();
								ckout<<"*****************  Time (in seconds) = " << (stopTime - startTime)<<"*****************"<<endl;
								CkExit();
						}
				}
				void ReductionCallback(CkReductionMsg * msg)
				{
						int data = *((int *)(msg->getData()));
						stopTime = CmiWallTimer();
						ckout<<"*****************  Time (in seconds) = " << (stopTime - startTime)<<"*****************"<<endl;

				//		ckout<<"Reduction Succesful : processor done ="<<data<<endl;
						CkExit();
				}*/
					

				void StepComplete(int subphase)
				{
						count++;
						if(count == CkNumPes())
						{
								count =0;
								if(subphase == 1)
								{
										workerProxy.Scale();
								}
								else if(subphase ==2)
								{
										workerProxy.Add();
								}
								else if(subphase ==3)
								{
										workerProxy.Triad();
								}
								else {
										numOfPhasesDone++;
										if(numOfPhasesDone < numIterations)
												{
														workerProxy.Copy();
												}
										else {
												//Done();
												CkExit();
										}
								}

						}
				}
};

class Worker:public CBase_Worker
{
		private:
				double a[N],b[N],c[N];
			

		public:
				Worker()
				{
					   for (int j=0; j<N; j++) {
							   a[j] = 1.0;
							   b[j] = 2.0;
							   c[j] = 0.0;
							   }
					
				}
				void Copy()
				{
						for (int j=0; j<N; j++)
								c[j] = a[j];

					//	CkPrintf("Worker %d Copy\n ",CkMyPe());
						int subphase = 1;
						controllerProxy.StepComplete(subphase);
				}
				void Scale()
				{
						for (int j=0; j<N; j++)
								b[j] = scalar * c[j];

					//	CkPrintf("Worker %d Scale\n ",CkMyPe());
						int subphase = 2;
						controllerProxy.StepComplete(subphase);
				}
				void Add()
				{
						for (int j=0; j<N; j++)
								c[j] = a[j] + b[j];

					//	CkPrintf("Worker %d Add\n ",CkMyPe());
						int subphase = 3;
						controllerProxy.StepComplete(subphase);
				}
				void Triad()
				{
						for (int j=0; j<N; j++)
								a[j] = b[j] + scalar * c[j];

						CkPrintf("Worker %d Triad\n ",CkMyPe());
						int subphase = 4;
						controllerProxy.StepComplete(subphase);
				}

				Worker(CkMigrateMessage * msg){}
};

#include "stream.def.h"



		

