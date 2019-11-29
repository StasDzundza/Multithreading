#include"Tests.h"

int main() {
	tests::registration_test();
	tests::unregistration_test();
	tests::reset_test();
	tests::lock_unlock_test();
	tests::try_lock_test();

	return 0;
}