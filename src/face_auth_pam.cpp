#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include "test.hpp"

PAM_EXTERN int pam_sm_setcred(pam_handle_t * pamh, int flags, int argc, const char** argv)
{
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t * pamh, int flags, int argc, const char** argv)
{
	return PAM_SUCCESS;
}

// argv 1 : choice of model, argv 2 : data_dir  argv 3: threshold   argv 4 : timeout
PAM_EXTERN int pam_sm_authenticate(pam_handle_t * pamh, int flags, int argc, const char** argv)
{
    int retval;
    const char *name;
    retval = pam_get_user(pamh, &name, "login: ");
    if (retval == PAM_SUCCESS) {
        std::cout << "username obtained: " << name << "\n";
    } else {
        std::cout << "trouble reading username\n";
        return retval;
    }

    std::string username(name);
    std::string model_dir_path(argv[1]);
    if (model_dir_path[model_dir_path.length() - 1] != '/')
        model_dir_path += "/";
    Test *obj = nullptr;
    if (std::stoi(argv[0])) {
        obj = new Test(std::stod(argv[2]), std::stoi(argv[3]), model_dir_path, username, model_dir_path + "haarcascade_frontalface_alt.xml");
    } else {
        obj = new Test(std::stod(argv[2]), std::stoi(argv[3]), model_dir_path, username, model_dir_path + "deploy.prototxt", model_dir_path + "res10_300x300_ssd_iter_140000_fp16.caffemodel");
    }
    
    if (obj->testFaces()) {
        //std::cout << "face matches";
        return PAM_SUCCESS;
    } else {
        //std::cout << "face doesn't match";
        return PAM_AUTH_ERR;
    }

    return PAM_AUTH_ERR;    
}