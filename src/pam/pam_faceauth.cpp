#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <faceauth/test.hpp>
#include <systemd/sd-journal.h>

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
    if (retval != PAM_SUCCESS) {
        std::cerr << "ERROR: trouble reading username\n";
        return retval;
    }

    std::string username(name);
    std::cout << "[faceauth] Attempting face auth for: " << username << "\n";
    sd_journal_print(LOG_INFO, "Initializing facial authentication for %s", name);
    std::string model_dir_path(argv[1]);
    if (model_dir_path[model_dir_path.length() - 1] != '/')
        model_dir_path += "/";
    Test *obj = nullptr;
    if (std::stoi(argv[0]) == HAARCASCADE) {
        obj = new Test(std::stod(argv[2]), std::stoi(argv[3]), model_dir_path, username, "haarcascade_frontalface_alt.xml");
    } else if (std::stoi(argv[0]) == DNN) {
        obj = new Test(std::stod(argv[2]), std::stoi(argv[3]), model_dir_path, username, "deploy.prototxt", "res10_300x300_ssd_iter_140000_fp16.caffemodel");
    }
    
    if (obj->testFaces()) {
        //std::cout << "face matches";
        sd_journal_print(LOG_NOTICE, "Face matches \\o/, unlocking....");
        return PAM_SUCCESS;
    } else {
        //std::cout << "face doesn't match";
        sd_journal_print(LOG_ERR, "Face didn't match :(");
        return PAM_AUTH_ERR;
    }

    return PAM_AUTH_ERR;    
}