#ifndef __NRP_ALIGNMENT_H_INCLUDED__
#define __NRP_ALIGNMENT_H_INCLUDED__

enum TypeAlign
{
    //! Aligned to parent's top or left side (default)
    alignUpperLeft=0,
    //! Aligned to parent's bottom or right side
    alignLowerRight,
    //! Aligned to the center of parent
    alignCenter,
    //! Stretched to fit parent
    alignScale,
    //! 
    alignAuto
};

#endif // __NRP_ALIGNMENT_H_INCLUDED__

