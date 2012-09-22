/*                                                        
*  quadtree.h                                              
*  - Siddhartha gupta                                                         
*  See detailed description at http://web.engr.illinois.edu/~gupta49/CS225/                                                                                            
*/

#ifndef QUADTREE_H
#define QUADTREE_H

#include "EasyBMP.h"

class Quadtree
{
public:

    Quadtree();                                   	    // default constructor
   
    Quadtree(BMP const &object, int d);				    // Parametrized constructor


    Quadtree (Quadtree const &source);              		//copy constructor

    Quadtree const & operator=(Quadtree const & source);    	//assignment overload function
 
    ~Quadtree();										    	// Destructor
 
    void buildTree(BMP &object, int d);	

    void printTree() const;
    BMP decompress() const;
    void prune(int tolerance);
	void clockwiseRotate();
	int pruneSize(int tolerance)const ;
	int idealPrune(int numleaves)const;
	RGBApixel getPixel(int x, int y)const;
	
   // operator= (This method relies on the private helper method compareTrees())
   bool operator==(Quadtree const & other) const;


private:

   // A simple class representing a single node of a Quadtree
   
   class QuadtreeNode
   {
   public:

      QuadtreeNode* nwChild;  // pointer to northwest child
      QuadtreeNode* neChild;  // pointer to northeast child
      QuadtreeNode* swChild;  // pointer to southwest child
      QuadtreeNode* seChild;  // pointer to southeast child

      RGBApixel element;      // the pixel stored as this node's "data"
	
   };

    QuadtreeNode* root;    // pointer to root of quadtree
	int D;				   // private member to store the resolution of the image passed in


    void newtree(QuadtreeNode *& subroot, const BMP &object, int d, int x, int y);			//helper function for constructor
																							//and buildtree()

    void removetree(QuadtreeNode *&subroot);												//helper function for destructor

	void copytree(QuadtreeNode *&subroot, QuadtreeNode const * sourceroot );				//helper function for copy constructor
																							//and assignment overloaded operator
	void rotate(QuadtreeNode *&subroot);													//helper function for clockwise rotate

	void prune(QuadtreeNode *& subroot, int tolerance);										//helper function for prune()

	bool toprune(QuadtreeNode * subroot, QuadtreeNode *parent, int tolerance) const;		//helper function for prune()
	
	int countleaves(QuadtreeNode *subroot) const;											//helped function used in prune()
	int prunecount(QuadtreeNode * subroot, int tolerance) const;							//helper function for pruneSize()
	int idealPrune(int min, int max, int numLeaves) const;									//helper function for idealPrune()

	RGBApixel findPix(int x, int y, QuadtreeNode const *subroot, int a, int b, int d) const; //helper function to recursively find the pixel.

    void printTree(QuadtreeNode const *current, int level) const;                            // printTree (private helper) - prints the contents of the Quadtree using a preorder traversal

    bool compareTrees(QuadtreeNode const *firstPtr, QuadtreeNode const *secondPtr) const;

};

#endif
