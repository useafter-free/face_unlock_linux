#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <iostream>
#include <opencv2/opencv.hpp>

const struct pam_conv conv = {
	misc_conv,
	NULL
};

int main(int argc, char * argv[])
{
	pam_handle_t * pamh = NULL;
	int retval;
	const char * user = NULL;

	retval = pam_start("faceauth_test", user, &conv, &pamh);

	// Are the credentials correct?
	if (retval == PAM_SUCCESS)
	{
		retval = pam_authenticate( pamh, 0 );
	}

	// Can the accound be used at this time?
	if (retval == PAM_SUCCESS)
	{
		std::cout <<  "Account is valid.\n";
		retval = pam_acct_mgmt( pamh, 0 );
	}

	// Did everything work?
	if (retval == PAM_SUCCESS)
	{
		std::cout <<  "Authenticated\n";
	}
	else
	{
		std::cout <<  "Not Authenticated\n";
	}

	if (pam_end( pamh, retval ) != PAM_SUCCESS)
	{
		pamh = NULL;
		std::cout <<  "check_user: failed to release authenticator\n";
		exit( 1 );
	}

	return retval == PAM_SUCCESS ? 0 : 1;
}