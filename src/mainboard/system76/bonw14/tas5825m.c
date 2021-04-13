#include <delay.h>
#include <drivers/i2c/tas5825m/tas5825m.h>

#define R(F) { \
	res = F; \
	if (res < 0) return res; \
}

#define amp_write_at(A, V) R(tas5825m_write_at(dev, A, V))

#define amp_write_block_at(A, ...) { \
	const uint8_t _values[] = __VA_ARGS__; \
	R(tas5825m_write_block_at(dev, A, _values, ARRAY_SIZE(_values))); \
}

#define amp_set_page(P) R(tas5825m_set_page(dev, P))

#define amp_set_book(B) R(tas5825m_set_book(dev, B))

#include "tas5825m-normal.c"
#include "tas5825m-sub.c"

int tas5825m_setup(struct device * dev, int id) {
	if (id == 0) {
		return tas5825m_setup_normal(dev);
	} else if (id == 1) {
		return tas5825m_setup_sub(dev);
	} else {
		return -1;
	}
}
