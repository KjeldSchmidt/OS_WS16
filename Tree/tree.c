#include <stdlib.h>
#include "tree.h"

/*
 *  Create a new tree. Every node is a tree in itself - so call this, when creating nodes, too!
 *  Argument rootValue: Integer to be inserted at the root node.
 *  Return: Pointer to root node.
 */
node_t *createTree(int rootValue){
  node_t *root = malloc( sizeof( node_t ) );  // Allocate memory for a node.
  root->value = rootValue;                    // Assign the required value
  return root;                                // Return a pointer to the node.
}

/*
 *  Insert a new value into the tree.
 *  This function automatically keeps the search tree structure.
 *  Argument *tree: Pointer to the tree
 *  Argument value: Integer value to be inserted into the tree.
 */
void insert(node_t *tree, int value){
  if ( tree->value < value ) {              // If the value-to-be-inserted is smaller than the value at the current node, go left.
    if ( tree->left == NULL ) {             // If the left child node is currently not set...
      tree->left = createTree( value );     // Set it to a new tree with the value-to-be-inserted
      tree->left->parent = tree;            // And set the current tree as he parent to the new tree! That's required later for cleaning up the tree.
    } else {
      insert( tree->left, value );          // If there already is a node, recursively call insert on it.
    }
  } else if ( tree->value > value ) {       // exactly the same thing, but for the right side.
    if ( tree->right == NULL ) {
      tree->right = createTree( value );
      tree->right->parent = tree;
    } else {
      insert( tree->right, value );
    }
  }                                         // Note: If the value-to-be-inserted is already inside the tree, absolutely nothing happens - it is not inserted again!
}

/*
 *  Search for an integer value in the tree.
 *  Argument *tree: Pointer to the tree to be searched.
 *  Argument value: Integer we're searching for.
 *  Returns 1 if it exists, 0 if not.
 */

int binarySearch(node_t *tree, int value){
  if ( tree->value == value ) {                         // Well, if the value is at the current node, then clearly,
    return 1;                                           // we've found the value and return 1.
  }
  
  if ( tree->left != NULL ) {                           // If not, see if a left node exists, and if yes, search it recursively.
    if ( binarySearch( tree->left, value ) == 1 ) {     // If we find it...
      return 1;                                         // Return 1! Note: We cannot return the result of binary seach directly, for even if it is 0, it might be in the right branch.
    }
  }

  if ( tree->right != NULL ) {                          // Same as above.
    if ( binarySearch( tree->right, value ) == 1 ) {    // Note: This time we _could_ actually just return the result straight
      return 1;                                         // But for readability, we leave the return 0 to the very end of the function.
    }
  }

  return 0;                                             // Do this if the value is nowhere to be found.
}


/*
 * Free all memory used by the tree
 * Argument *tree: Pointer to the tree to be cleaned.
 */
void cleanUpTree(node_t *tree) {
  if ( tree->right != NULL ) {    // If the right branch isn't already a nullpointer...
    cleanUpTree( tree->right );   // Recursively call cleanUpTree on the right branch...
    tree->right = NULL;           // And make it a nullpointer now!
  }

  if ( tree->left != NULL ) {     // Same as above, but for the left branch.
    cleanUpTree( tree->left );
    tree->left = NULL;
  }

  free( tree );                   // Free the node. This doesn't have to be set to zero - this is done by the user calling this function.
}
