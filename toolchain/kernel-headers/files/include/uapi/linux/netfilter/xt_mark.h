#ifndef _XT_MARK_H
#define _XT_MARK_H

#include <linux/types.h>

struct xt_mark_tginfo2 {
	__u32 mark, mask;
#if 1 /* ZyXEL add the following section for suitable size between kernel space and user space.*/
    __u8 mode;
#endif
};

struct xt_mark_mtinfo1 {
	__u32 mark, mask;
	__u8 invert;
};

#endif /*_XT_MARK_H*/
