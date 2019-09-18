//********************************************************************
//		        Matt Stanley
//		     3013 - Algorithms
//		Assignment 1: Balanced Trees
//			 9/18/2019
//********************************************************************
// This program reads in a set of values from an input file and sorts 
// them in a way that will produce a sorted or balanced binary tree.
// In order to sort the values, two vectors are first created. The 
// first of these vectors holds the values from the input file but
// sorted in ascending order. Once the first vector's contents are
// sorted, it is then passed to a function that will pick out its 
// values in a certain order and insert them into the second vector.
// This second vector's values will then be passed into a binary tree.
// If the values in the vector are in the right order, the binary tree
// should end up either balanced, full, or both. Once the tree has
// been created, its contents are used to make a graphviz output file
// that will allow the user to clearly see the tree's structure.
//********************************************************************
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct node
{
	int data;
	node *left;
	node *right;

	// Default constructor
	node()
	{
		data = -1;
		left = nullptr;
		right = nullptr;
	}
	node(int x)
	{
		data = x;
		left = nullptr;
		right = nullptr;
	}

};

class BSTree
{
private:
	node *root;

	int count(node *root)
	{
		// If there are no children, return
		if (!root)
		{
			return 0;
		}

		else
		{
			return 1 + count(root->left) + count(root->right);
		}

	}

	void insert(node *&root, node *&temp)
	{
		// Will insert once a spot has been found
		if (!root)
		{
			root = temp;
		}

		else
		{
			if (temp->data < root->data)
			{
				insert(root->left, temp);
			}

			else
			{
				insert(root->right, temp);
			}

		}

	}

	void print_node(node *n, string label = "")
	{
		if (label != "")
		{
			cout << "[" << label << "]";
		}
		cout << "[[" << n << "][" << n->data << "]]\n";

		if (n->left)
		{
			cout << "\t|-->[L][[" << n->left << "][" << n->left->data << "]]\n";
		}

		else
		{
			cout << "\t\\-->[L][null]\n";
		}

		if (n->right)
		{
			cout << "\t\\-->[R][[" << n->right << "][" << n->right->data << "]]\n";
		}

		else
		{
			cout << "\t\\-->[R][null]\n";
		}

	}

	node *minValueNode(node *root)
	{
		node *current = root;

		if (root->right)
		{
			current = root->right;
			while (current->left != nullptr)
			{
				current = current->left;
			}
		}
		else if (root->left)
		{
			current = root->left;
			while (current->right != nullptr)
			{
				current = current->right;
			}
		}

		return current;
	}

	// Finds the height of the tree
	int height(node *root)
	{
		if (!root)
		{
			return 0;
		}
		else
		{
			int left = height(root->left);
			int right = height(root->right);
			if (left > right)
			{
				return left + 1;
			}
			else
			{
				return right + 1;
			}

		}

	}

	void printGivenLevel(node *root, int level)
	{
		// If the tree is empty return nothing.
		if (root == nullptr)
			return;

		// This will recursively print the first node of the tree
		if (level == 1)
		{
			print_node(root);
		}
		else if (level > 1)
		{
			printGivenLevel(root->left, level - 1);
			printGivenLevel(root->right, level - 1);
		}
	}

	//************************************************************************
	 // Method to help create GraphViz code so the expression tree can
	 // be visualized. This method prints out all the unique node id's
	 // by traversing the tree.
	 // Recivies a node pointer to root and performs a simple recursive
	 // tree traversal.
	 //************************************************************************
	void GraphVizGetIds(node *nodePtr, ofstream &VizOut)
	{
		static int NullCount = 0;
		if (nodePtr)
		{
			GraphVizGetIds(nodePtr->left, VizOut);
			VizOut << "node" << nodePtr->data
				<< "[label=\"" << nodePtr->data << "\\n"
				//<<"Add:"<<nodePtr<<"\\n"
				//<<"Par:"<<nodePtr->parent<<"\\n"
				//<<"Rt:"<<nodePtr->right<<"\\n"
				//<<"Lt:"<<nodePtr->left<<"\\n"
				<< "\"]" << endl;
			if (!nodePtr->left)
			{
				NullCount++;
				VizOut << "nnode" << NullCount << "[label=\"X\",shape=point,width=.15]" << endl;
			}
			GraphVizGetIds(nodePtr->right, VizOut);
			if (!nodePtr->right)
			{
				NullCount++;
				VizOut << "nnode" << NullCount << "[label=\"X\",shape=point,width=.15]" << endl;
			}
		}
	}

	//************************************************************************
	// This method is partnered with the above method, but on this pass it
	// writes out the actual data from each node.
	// Don't worry about what this method and the above method do, just
	// use the output as your told:)
	//************************************************************************
	void GraphVizMakeConnections(node *nodePtr, ofstream &VizOut)
	{
		static int NullCount = 0;
		if (nodePtr)
		{
			GraphVizMakeConnections(nodePtr->left, VizOut);
			if (nodePtr->left)
				VizOut << "node" << nodePtr->data << "->"
				<< "node" << nodePtr->left->data << endl;
			else
			{
				NullCount++;
				VizOut << "node" << nodePtr->data << "->"
					<< "nnode" << NullCount << endl;
			}

			if (nodePtr->right)
				VizOut << "node" << nodePtr->data << "->"
				<< "node" << nodePtr->right->data << endl;
			else
			{
				NullCount++;
				VizOut << "node" << nodePtr->data << "->"
					<< "nnode" << NullCount << endl;
			}

			GraphVizMakeConnections(nodePtr->right, VizOut);
		}
	}

public:
	// Default constructor
	BSTree()
	{
		root = nullptr;
	}
	// Destructor
	~BSTree()
	{

	}

	// This public count function will call the private count function
	int count()
	{
		return count(root);
	}

	void insert(int x)
	{
		node *temp = new node(x);
		insert(root, temp);
	}

	void mainValue()
	{
		print_node(minValueNode(root), "minVal");
	}

	int height()
	{
		return height(root);
	}

	int top()
	{
		if (root)
			return root->data;

		else
			return 0;
	}

	/* Function to line by line print level order traversal a tree*/
	void printLevelOrder()
	{
		cout << "Begin Level Order===================\n";
		int h = height(root);
		int i;
		for (i = 1; i <= h; i++)
		{
			printGivenLevel(root, i);
			cout << "\n";
		}
		cout << "End Level Order===================\n";
	}

	//************************************************************************
	// Recieves a filename to place the GraphViz data into.
	// It then calls the above two graphviz methods to create a data file
	// that can be used to visualize your expression tree.
	//************************************************************************
	void GraphVizOut(string filename)
	{
		ofstream VizOut;
		VizOut.open(filename);
		VizOut << "Digraph G {\n";
		GraphVizGetIds(root, VizOut);
		GraphVizMakeConnections(root, VizOut);
		VizOut << "}\n";
		VizOut.close();
	}
};


// Function Prototype
void BubbleSort(vector <int> &vect);
void BSTSort(vector <int> &, vector <int> &);


int main()
{
	// Create a binary search tree object
	BSTree B;

	int value;

	// Create and open an input file
	ifstream infile;
	infile.open("values.txt");

	string inputfile = "before.txt";

	// Initialize the vectors
	vector<int> values;
	vector <int> BSTvalues;

	// Read in first value
	infile >> value;
	values.push_back(value);

	// Read values until end of file is reached
	while (!infile.eof())
	{
		infile >> value;
		values.push_back(value);
	}

	// Sort the vector in ascending order
	BubbleSort(values);

	// Sort the vector in a way it will be full
	// or balanced
	BSTSort(values, BSTvalues);
	
	// Insert the new vector's values into the tree
	for (int i = 0; i < BSTvalues.size(); i++)
	{
		B.insert(BSTvalues[i]);
	}

	// Show the order of the new vector
	for (int i = 0; i < BSTvalues.size(); i++)
	{
		cout << BSTvalues[i] << " ";
	}
	cout << endl;

	// Show a visualization of the binary tree through both
	// code and graphviz.
	B.printLevelOrder();
	B.GraphVizOut(inputfile);

	return 0;
}

// ********************************************************
// This function will use Bubble Sort to sort the values
// from the input file in ascending order.

// Parameters: A vector passed in by reference. This 
// is done so the original vector is changed
//*********************************************************
void BubbleSort(vector <int> &vect)
{
	int temp;
	bool swap;

	do
	{
		swap = false;
		for (int i = 0; i < vect.size() - 1; i++)
		{
			// Compare the current value to the one after
			if (vect[i] > vect[i + 1])
			{
				temp = vect[i];
				vect[i] = vect[i + 1];
				vect[i + 1] = temp;
				swap = true;
			}
		}
	} while (swap == true);
}

//**************************************************************
// This function will take in the sorted vector and insert these
// values into a new vector. The values in the new vector
// should produce a full or balanced binary tree when read in order
// from left to right.
//
// Parameters: two vectors are passed to this function by
// reference. 
// *************************************************************
void BSTSort(vector<int> &sortedvect, vector<int> &newvect)
{
	// Initialize the necessary parameters
	int size = 0;
	int middle;
	int count = 0;
	size = sortedvect.size();
	middle = size / 2;
	
	// Find out whether there are an odd or even number
	// of nodes to the left and right of middle node.
	for (int i = 0; i < middle; i++)
	{
		count++;
	}

	// If there are an even number of nodes, move over
	// one.
	if (count % 2 == 0)
	{
		middle = middle - 1;
	}

	// Insert the root node
	newvect.push_back(sortedvect[middle]);
	sortedvect[middle] = 0;

	// Get every other value from vect but in reverse.
	for (int i = sortedvect.size() - 2; i >=0; i = i-2)
	{
		// Receive the value if it has not been used yet
		if (sortedvect[i] != 0)
		{
			newvect.push_back(sortedvect[i]);
			sortedvect[i] = 0;
		}
	}

	// Put the rest of the values into the new vector
	for (int i = 0; i < sortedvect.size(); i++)
	{
		if (sortedvect[i] != 0)
		{
			newvect.push_back(sortedvect[i]);
			sortedvect[i] = 0;
		}
	}


}
