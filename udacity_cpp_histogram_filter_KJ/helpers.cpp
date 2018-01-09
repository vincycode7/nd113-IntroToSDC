/**
	helpers.cpp

	Purpose: helper functions which are useful when
	implementing a 2-dimensional histogram filter.

	This file is incomplete! Your job is to make the
	normalize and blur functions work. Feel free to
	look at helper.py for working implementations
	which are written in python.
*/

#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
// #include "debugging_helpers.cpp"

using namespace std;

// define our types for clearer code
// a vector of chars
typedef vector <char> vectorChar;

// a grid of chars
typedef vector <vectorChar> gridChar;

// a vector of floats
typedef vector <float> vectorFloat;

// a grid of floats
typedef vector <vectorFloat> gridFloat;


/**
	TODO - implement this function

    Normalizes a grid of numbers.

    @param grid - a two dimensional grid (vector of vectors of floats)
		   where each entry represents the unnormalized probability
		   associated with that grid cell.

    @return - a new normalized two dimensional grid where the sum of
    	   all probabilities is equal to one.
*/
gridFloat normalize(gridFloat grid) {

	gridFloat newGrid;

	// todo - your code here
	int height = grid.size();
	int width = grid[0].size();

	float normalizer = 0;
	// work out normalizer
	for (int row=0; row < height; row++) {
			for (int col=0; col < width; col++) {
				normalizer += grid[row][col];
			}
	}

	// apply normalizer
	vectorFloat newRow;
	for (int row=0; row < height; row++) {
			newRow.clear();
			for (int col=0; col < width; col++) {
				newRow.push_back(grid[row][col]/normalizer);
			}

			newGrid.push_back(newRow);
	}

	return newGrid;
}

/**
	TODO - implement this function.

    Blurs (and normalizes) a grid of probabilities by spreading
    probability from each cell over a 3x3 "window" of cells. This
    function assumes a cyclic world where probability "spills
    over" from the right edge to the left and bottom to top.

    EXAMPLE - After blurring (with blurring=0.12) a localized
    distribution like this:

    0.00  0.00  0.00
    0.00  1.00  0.00
    0.00  0.00  0.00

    would look like this:

	0.01  0.02	0.01
	0.02  0.88	0.02
	0.01  0.02  0.01

    @param grid - a two dimensional grid (vector of vectors of floats)
		   where each entry represents the unnormalized probability
		   associated with that grid cell.

	@param blurring - a floating point number between 0.0 and 1.0
		   which represents how much probability from one cell
		   "spills over" to it's neighbors. If it's 0.0, then no
		   blurring occurs.

    @return - a new normalized two dimensional grid where probability
    	   has been blurred.
*/
gridFloat blur(gridFloat grid, float blurring) {

	gridFloat newGrid;

	// your code here

	int height = grid.size();
	int width = grid[0].size();

	float center_prob = 1.0-blurring;
	float corner_prob = blurring / 12.0;
	float adjacent_prob = blurring / 6.0;

	// establish window grid distribution
	gridFloat window;
	vectorFloat windowRow1 = {corner_prob, adjacent_prob, corner_prob};
	vectorFloat windowRow2 = {adjacent_prob, center_prob, adjacent_prob};
	vectorFloat windowRow3 = {corner_prob, adjacent_prob, corner_prob};

	window.push_back(windowRow1);
	window.push_back(windowRow2);
	window.push_back(windowRow3);

	// zero the new grid
	newGrid = gridFloat (height, vectorFloat (width, 0));

	float gridVal, mult;
	int blurRow, blurCol;
	for (int row=0; row < height; row++) {
		for (int col=0; col < width; col++) {
			// save original value
			gridVal = grid[row][col];
			for (int dx=-1; dx<2; dx++) {
				for (int dy=-1; dy<2; dy++) {
					mult = window[dx+1][dy+1];
					// C++ modulo: (a%b+b)%b
					blurRow = ((row + dy) % height + height) % height;
					blurCol = ((col + dx) % width + width) % width;
					newGrid[blurRow][blurCol] += mult * gridVal;
				}
			}
		}
	}

	return normalize(newGrid);
}

/** -----------------------------------------------
#
#
#	You do not need to modify any code below here.
#
#
# ------------------------------------------------- */


/**
    Determines when two grids of floating point numbers
    are "close enough" that they should be considered
    equal. Useful for battling "floating point errors".

    @param g1 - a grid of floats

    @param g2 - a grid of floats

    @return - A boolean (True or False) indicating whether
    these grids are (True) or are not (False) equal.
*/
bool close_enough(gridFloat g1, gridFloat g2) {
	int i, j;
	float v1, v2;
	if (g1.size() != g2.size()) {
		return false;
	}

	if (g1[0].size() != g2[0].size()) {
		return false;
	}
	for (i=0; i<g1.size(); i++) {
		for (j=0; j<g1[0].size(); j++) {
			v1 = g1[i][j];
			v2 = g2[i][j];
			if (abs(v2-v1) > 0.0001 ) {
				return false;
			}
		}
	}
	return true;
}

bool close_enough(float v1, float v2) {
	if (abs(v2-v1) > 0.0001 ) {
		return false;
	}
	return true;
}

/**
    Helper function for reading in map data

    @param s - a string representing one line of map data.

    @return - A row of chars, each of which represents the
    color of a cell in a grid world.
*/
vector <char> read_line(string s) {
	vector <char> row;

	size_t pos = 0;
	string token;
	string delimiter = " ";
	char cell;

	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());

		cell = token.at(0);
		row.push_back(cell);
	}

	return row;
}

/**
    Helper function for reading in map data

    @param file_name - The filename where the map is stored.

    @return - A grid of chars representing a map.
*/
vector < vector <char> > read_map(string file_name) {
	ifstream infile(file_name);
	vector < vector <char> > map;
	if (infile.is_open()) {

		char color;
		vector <char> row;

		string line;

		while (std::getline(infile, line)) {
			row = read_line(line);
			map.push_back(row);
		}
	}
	return map;
}

/**
    Creates a grid of zeros

    For example:

    zeros(2, 3) would return

    0.0  0.0  0.0
    0.0  0.0  0.0

    @param height - the height of the desired grid

    @param width - the width of the desired grid.

    @return a grid of zeros (floats)
*/
gridFloat zeros(int height, int width) {
	int i, j;
	gridFloat newGrid;
	vector <float> newRow;

	for (i=0; i<height; i++) {
		newRow.clear();
		for (j=0; j<width; j++) {
			newRow.push_back(0.0);
		}
		newGrid.push_back(newRow);
	}
	return newGrid;
}

// int main() {
// 	vector < vector < char > > map = read_map("maps/m1.txt");
// 	show_grid(map);
// 	return 0;
// }