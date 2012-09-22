/*                                                        
*  quadtree.cpp                                              
*  - Siddhartha gupta                                                         
*  See detailed description at http://web.engr.illinois.edu/~gupta49/CS225/                                                                                            
*/

#include <iostream>

using namespace std;

#include "quadtree.h"
#include "EasyBMP.h"


Quadtree::Quadtree()
{
	root=NULL;
}

Quadtree::Quadtree(BMP const &object, int d)					//parametrized constructor
{
	D=d;														//created private member D, to store the resolution
	newtree(root, object, d, 0, 0);								//of the image

}

/* Helper function for constructor above and buildtree()
*/

void Quadtree::newtree(QuadtreeNode *& subroot, const BMP &object, int d, int x, int y)
{																


	if (d==1)													//base case for recursion, if recurse upto one pixel, 
																//store it in the Quadtree leaves.
	{
	
	  subroot  = new QuadtreeNode;								//assigning memory in the heap for the leaves.
	  subroot->element=*(object.operator()(x, y));

      subroot->nwChild = NULL;									//Being a leaf, setting its children to NULL
      subroot->neChild = NULL;
      subroot->swChild = NULL; 
      subroot->seChild = NULL;

	}

	else 
	{	
    subroot  = new QuadtreeNode;								//assigning memory in the heap for the nodes.

	newtree(subroot->nwChild, object, d/2,x,y);					//recursing further to each of the children, with 
	newtree(subroot->neChild, object, d/2,x+d/2, y);			//new reference points (x,y) accordingly 
	newtree(subroot->swChild, object, d/2, x, y +d/2);
	newtree(subroot->seChild, object, d/2, x+d/2, y +d/2);
	
	// Setting nodes to hold the average of the colors of its immediate children

	subroot->element.Red = (subroot->nwChild->element.Red+subroot->neChild->element.Red+subroot->swChild->element.Red+subroot->seChild->element.Red)/4;
	subroot->element.Green = (subroot->nwChild->element.Green+subroot->neChild->element.Green+subroot->swChild->element.Green+subroot->seChild->element.Green)/4;

	subroot->element.Blue = (subroot->nwChild->element.Blue+subroot->neChild->element.Blue+subroot->swChild->element.Blue+subroot->seChild->element.Blue)/4;


	}

}

void Quadtree:: buildTree(BMP &object, int d)
{
	if (root!=NULL)
	{
		removetree(root);				//Delete the current contents of the Quadtree object
	}

	D=d;								//setting the private member with the resolution of the image
	newtree(root, object,d, 0, 0);
}


				

Quadtree::Quadtree (Quadtree const &source)
{
	D=source.D;
	copytree(root, source.root);	
}



/* 
Recursive helper function for the copy constructor and overloaded assignment operator
*/
void Quadtree::copytree(QuadtreeNode *& subroot,QuadtreeNode const * sourceroot )
{
	if (sourceroot != NULL) 
	{
	// Copy this node and its children	

	subroot = new QuadtreeNode;							//create space for the new node
	subroot->element = sourceroot->element;

	copytree(subroot->nwChild, sourceroot->nwChild);
	copytree(subroot->neChild, sourceroot->neChild);
	copytree(subroot->swChild, sourceroot->swChild);
	copytree(subroot->seChild, sourceroot->seChild);
	}
	
	else 
		subroot=NULL;
	
}


Quadtree const & Quadtree::operator=(Quadtree const & source)
{
	if(this==&source)				//check for self-assignment
		return *this;
	
	removetree(root);				//remove anything that is in the destination tree

	D=source.D;
	copytree(root, source.root);
	
	return *this;
		
}



RGBApixel Quadtree::getPixel(int x, int y) const
{
	if (x>D||y>D||root==NULL)
		return RGBApixel();

	return findPix(x, y, root, 0, 0, D);
}

/* Helper function to recursively find the pixel.
*/
RGBApixel Quadtree::findPix(int x, int y, QuadtreeNode const *subroot, int a, int b, int d) const
{
	//a and b are reference points, initially passed in as (0,0)
	
	if ((subroot->neChild==NULL)||(d==1))
		return subroot->element;								//if u find the pixel, return it
	
	else 
	{
		if((x<a+d/2)&&(x>=a)&&(y<b+d/2)&&(y>=b))				//Northwest block of the image
		{
		return findPix(x, y, subroot->nwChild,a, b, d/2);
		}

		else if((x>=a+d/2)&&(x<a+d)&&(y<b+d/2)&&(y>=b))			//Northeast block of the image
		{		
		return findPix(x, y, subroot->neChild,a+d/2, b, d/2);	//reference point changes, shifts forwards.
		}
		
		else if((x<a+d/2)&&(x>=a)&&(y>=b+d/2)&&(y<=b+d))		//Southwest block of the image
		{
		return findPix(x, y, subroot->swChild,a, b+d/2, d/2);	//reference point changes, shifts downwards.
		}

		else
		{
		return findPix(x, y, subroot->seChild,a+d/2, b+d/2, d/2);	//Southeast block of the image, reference point shifts
		}															//forward and downwards.
	}
		
}
	
BMP  Quadtree::decompress() const
{	
	BMP object;
	
	object.SetSize(D,D);					

	for ( int i=0;i<D;i++)
	{
		for (int j=0;j<D;j++)
			{
				*(object(i, j))=getPixel(i, j);
			}
	
	}
	return object;
}


Quadtree::~Quadtree()
{
	if (root!=NULL)
	removetree(root);

}

void Quadtree::removetree(QuadtreeNode *& subroot)
{
	if(subroot==NULL)   					
		return;
	
	else 
	{											//When node is not NULL, recurse for all children
		removetree(subroot->nwChild);
		removetree(subroot->neChild);
		removetree(subroot->swChild);
		removetree(subroot->seChild);
		delete subroot;							//get rid of the node.
		subroot=NULL;
	}		
}

void Quadtree::clockwiseRotate()
{
	if(root!=NULL)
		rotate(root);
}

void Quadtree::rotate(QuadtreeNode *&subroot)
{
	if(subroot->nwChild!=NULL)
{
		QuadtreeNode *temp = subroot->nwChild;
		subroot->nwChild=subroot->swChild;
		subroot->swChild=subroot->seChild;                      //rotating image by 90 degrees by swapping pointers
		subroot->seChild=subroot->neChild;
		subroot->neChild = temp;
	
		rotate(subroot->nwChild);
		rotate(subroot->neChild);
		rotate(subroot->swChild);
		rotate(subroot->seChild);
}
}
	
void Quadtree::prune(int tolerance){
	if (root!=NULL)
	prune(root, tolerance);
}

void Quadtree::prune(QuadtreeNode *& subroot, int tolerance)
{
	if (subroot!=NULL)
	{
		if ((toprune(subroot->neChild, subroot, tolerance))&&(toprune(subroot->nwChild, subroot, tolerance))
		&&(toprune(subroot->swChild, subroot, tolerance))&&(toprune(subroot->seChild, subroot, tolerance)))
			{
				//if the 'if' condition evaluates to be true, the toprune() function has asserted that all the leaves
				//of the root being tested is within the tolerance and thus the root's children must be pruned

				removetree(subroot->neChild);
				removetree(subroot->nwChild);
				removetree(subroot->seChild);
				removetree(subroot->swChild);
			}
			
		prune(subroot->neChild, tolerance);
		prune(subroot->nwChild, tolerance);
		prune(subroot->seChild, tolerance);
		prune(subroot->swChild, tolerance);
		
	}	
	
}

/*
This function returns true if and only if 'all' the leaves of the parent node passed in are within the tolerance level.
*/

bool Quadtree::toprune(QuadtreeNode * subroot, QuadtreeNode *parent, int tolerance) const
{
	    if (subroot==NULL)
		return false;						//If the leaves are already pruned, no need to prune again.

		int x;
		if(subroot->neChild==NULL)			//if one of the children is NULL, you have reached a leaf! so test if the 
											//leaf's colors lie within the tolerance level as given.
			{
				x= (subroot->element.Red-parent->element.Red)*(subroot->element.Red-parent->element.Red) +
					(subroot->element.Green-parent->element.Green)*(subroot->element.Green-parent->element.Green)
				+ (subroot->element.Blue-parent->element.Blue)*(subroot->element.Blue-parent->element.Blue);
			    
				if (x<=tolerance)
					return true;
				else return false;
			}

		//otherwise you recurse further until you reach a leaf
		else return (toprune(subroot->neChild, parent, tolerance))&&(toprune(subroot->nwChild, parent, tolerance))&&
					(toprune(subroot->seChild, parent, tolerance))&&(toprune(subroot->swChild, parent, tolerance));
	
}


int Quadtree::pruneSize(int tolerance)const
{
		if (root!=NULL)
		return D*D - prunecount(root, tolerance);
		
}


/* recursive function that returns the no. of leaves that will be pruned (not the number of leaves which will be left
after pruning) */
int Quadtree::prunecount(QuadtreeNode * subroot, int tolerance) const
{
	if (subroot!=NULL)
	{
		if ((toprune(subroot->neChild, subroot, tolerance))&&(toprune(subroot->nwChild, subroot, tolerance))
		&&(toprune(subroot->swChild, subroot, tolerance))&&(toprune(subroot->seChild, subroot, tolerance)))
			{
				return countleaves(subroot->neChild) + countleaves(subroot->nwChild) + countleaves(subroot->seChild) 
						+ countleaves(subroot->swChild) -1 ; 
									// Similar to the earlier prune function, however now instead of pruning,
									// I'm returning how many leaves would've been left had the pruning actually
									// taken place. The '-1' is for the root node, which itself becomes a leaf, after 
									// its children nodes are pruned, thus increasing the leaf count by 1.
			}

		return prunecount(subroot->neChild, tolerance)+	prunecount(subroot->nwChild, tolerance)+
		prunecount(subroot->seChild, tolerance)+ prunecount(subroot->swChild, tolerance);

	}	

	else
		return 0;
	
	
}


int Quadtree::countleaves(QuadtreeNode *subroot)const
{
	if (subroot==NULL)
		return 0;					//no leaves in an empty tree

	else if (subroot->neChild==NULL)
		return 1;
	else 
		return countleaves(subroot->neChild)+countleaves(subroot->nwChild)+countleaves(subroot->seChild)+countleaves(subroot->swChild);
}


int Quadtree::idealPrune(int numLeaves)const
{
		int	min=0;
		int max=255*255*3;
		return idealPrune(min, max, numLeaves);
}

int Quadtree::idealPrune(int min, int max, int numLeaves) const
{

		int leaves;
		if(min>max)
			return min;													//I asked a TA what we were supposed to return
																		//if we can't find the numLeaves being passed
		leaves=pruneSize((min+max)/2);									//in, he said just return min, since that 
																		//tolerance value will give the number of 
																		//leaves no more than numLeaves as required
	
		if (leaves==numLeaves)
		{
				if(leaves==pruneSize(((min+max)/2)-1))
			return idealPrune(0, ((min+max)/2)-1, numLeaves);			//finding the minimum tolerance out of all the
																		//tolerances that give the required number of 
			return (min+max)/2;											//leaves.
		}

		else if (leaves<numLeaves)
		   return idealPrune(min,((min+max)/2)-1, numLeaves); 			//basic binary search, if leaves is less than 
																		//the required number of leaves, we're pruning 
																		//too much, and the current tolerance is too 
																		//high, so redefine maximum = midpoint -1 
	
		else if(leaves>numLeaves)										//if leaves is more than 
			return idealPrune(((min+max)/2)+1, max, numLeaves);			//the required number of leaves, we're not 
																		// pruning enough, and the current tolerance is 
																		//too low, so redefine minimum = midpoint +1 

}

bool Quadtree::compareTrees(QuadtreeNode const * firstPtr,
                            QuadtreeNode const * secondPtr) const {
   if (firstPtr == NULL && secondPtr == NULL)
      return true;

   if (firstPtr == NULL || secondPtr == NULL)
      return false;

   // if they're both leaves, see if their elements are equal
   // note: child pointers should _all_ either be NULL or non-NULL,
   // so it suffices to check only one of each
   if (firstPtr->neChild == NULL && secondPtr->neChild == NULL) {
      if (firstPtr->element.Red != secondPtr->element.Red ||
         firstPtr->element.Green != secondPtr->element.Green ||
         firstPtr->element.Blue != secondPtr->element.Blue)
         return false;
      else
         return true;
   }

   // they aren't both leaves, so recurse
   return (compareTrees(firstPtr->neChild, secondPtr->neChild) &&
           compareTrees(firstPtr->nwChild, secondPtr->nwChild) &&
           compareTrees(firstPtr->seChild, secondPtr->seChild) &&
           compareTrees(firstPtr->swChild, secondPtr->swChild));
}