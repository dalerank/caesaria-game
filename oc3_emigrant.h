#ifndef __OPENCAESAR3_EMIGRANT_H_INCLUDE_
#define __OPENCAESAR3_EMIGRANT_H_INCLUDE_

#include "walker.hpp"

/** This is an immigrant coming with his stuff */
class Emigrant : public Walker
{
public:
	virtual Emigrant* clone() const;

	static Emigrant* create( const Road& startPoint );

	void onDestination();
	void getPictureList(std::vector<Picture*> &oPics);
	void onNewDirection();

	~Emigrant();
private:
	Emigrant();

	void assignPath( const Road& startPoint );
	class Impl;
	std::auto_ptr< Impl > _d;

	Picture& _getCartPicture();

};

#endif