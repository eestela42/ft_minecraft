#ifndef LINKTAB_HPP
# define LINKTAB_HPP


template <typename T>

class LinkTab {
	private :
		T *data;
		LinkTab *left = NULL;
		LinkTab *right = NULL;
		LinkTab *top = NULL;
		LinkTab *bottom = NULL;
		LinkTab** neighbors[4];
	public :
		LinkTab(T *data) : LinkTab(), data(data)
		{}

		LinkTab() {
			data = NULL;
			neighbors[0] = &left;
			neighbors[1] = &right;
			neighbors[2] = &top;
			neighbors[3] = &bottom;
			left = NULL;
			right = NULL;
			top = NULL;
			bottom = NULL;
		}
		
		~LinkTab() {
			// if (left)
			// 	delete left;
			// if (right)
			// 	delete right;
			// if (top)
			// 	delete top;
			// if (bottom)
			// 	delete bottom;
		}
		void setLeft(LinkTab *left) {
			this->left = left;
		}
		void setRight(LinkTab *right) {
			this->right = right;
		}
		void setTop(LinkTab *top) {
			this->top = top;
		}
		void setBottom(LinkTab *bottom) {
			this->bottom = bottom;
		}
		LinkTab *getLeft() {
			return left;
		}
		LinkTab *getRight() {
			return right;
		}
		LinkTab *getTop() {
			return top;
		}
		LinkTab *getBottom() {
			return bottom;
	}


};




#endif