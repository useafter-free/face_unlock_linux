## [WIP] Facial authentication for Linux

![GitHub](https://img.shields.io/github/license/useafter-free/face_unlock_linux) 

This project aims to create a facial authentication module and integrate it with the Pluggable authentication

module and Display managers to provide a seamless unlocking experience.

#### 0. Installing the dependencies

- git

- OpenCV 4
- other dependencies are already present in most Linux distributions. 

####  

#### 1. Building the project

```bash
git clone https://github.com/useafter-free/face_unlock_linux.git
cd face_unlock_linux
make
sudo make install 
**OR** 
make PREFIX=your_preffered_path install
```

#### 2. Training the model

- You need to put some images into data/**username**/ directory for training purpose.
- You can play around with the code in `src/train.cpp` and change it to fetch frames from camera.
- Note: Images not having any face or having > 1 faces will be rejected. 

```
cd examples
make train
./train `arg`
arg = 1, for using the haar cascade classifier
	= 2, for Deep neural net based classifier
```

This will save a `username`-model.xml file `data/` directory.

#### 3. Testing

- first, you need to create a pam config file `/etc/pam.d/faceauth_test`.

- An example config file can be:

  ```
  auth sufficient pam_faceauth.so 2 /etc/faceauth_data/ 104.0 10
  account sufficient pam_faceauth.so 2 /etc/faceauth_data/ 104.0 10
  ```

- Then compile the test binary:

  ```
  cd exaples
  make test
  ./test
  ```


- To see logs for the application, run  `journalctl -xe`

- If you want to use the facial unlock with display managers like LightDM, gdm e.t.c. or lockscreens like i3lock, then just add the config lines to the respective files in /etc/pam.d/

- For example:

  - For i3lock - /etc/pam.d/i3lock

    ```
    #
    # PAM configuration file for the i3lock-color screen locker. By default, it includes
    # the 'system-auth' configuration file (see /etc/pam.d/system-auth) for Arch and Gentoo
    # and 'login' for Debian. Note that vanilla i3lock upstream uses 'login' instead.
    #
    #
    auth sufficient pam_faceauth.so 2 /etc/faceauth_data/ 104.0 10
    auth include system-auth # For Arch/Gentoo
    #auth include login # For Debian
    ```

### Contributing

- Kindly note, this project is still work in progress, any type contributions are accepted.
- You can start off by looking at TODOs and issues.



**TODOs:**

1. Add a minimal GUI (Preferably using Qt) to test and train the model.
2. Store model more securely.
3. Improve the facial recognition.
4. Integrate the module with display managers (like LightDM, gdm, sddm e.t.c.).
5. Improve documentation.