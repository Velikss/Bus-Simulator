pipeline
{
    agent none
    stages
    {
        stage("Init")
        {
            parallel
            {
                stage('Linux')
                {
                    agent
                    {
                        label "Linux"
                    }
                    steps
                    {
                        script
                        {
                            sh 'git submodule update --init --recursive'
                        }
                    }
                }
                stage('Windows')
                {
                    agent
                    {
                        label "Windows"
                    }
                    steps
                    {
                        script
                        {
                            bat 'git submodule update --init --recursive'
                        }
                    }
                }
            }
        }
        stage("Build")
        {
            parallel
            {
                stage('Linux')
                {
                    agent
                    {
                        label "Linux"
                    }
                    steps
                    {
                        script
                        {
                            sh 'cmake -Dgtest_disable_pthreads=OFF .'
                            sh 'cmake --build .'
                        }
                    }
                }
                stage('Windows')
                {
                    agent
                    {
                        label "Windows"
                    }
                    steps
                    {
                        script
                        {
                            bat 'cmake -Dgtest_disable_pthreads=OFF -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake .'
                            bat 'cmake --build .'
                        }
                    }
                }
            }
        }
        stage("Test")
        {
            parallel
            {
                stage('Linux')
                {
                    agent
                    {
                        label "Linux"
                    }
                    steps
                    {
                        script
                        {
                            sh '/usr/bin/ctest -T test --verbose'
                        }
                    }
                }
                stage('Windows')
                {
                    agent
                    {
                        label "Windows"
                    }
                    steps
                    {
                        script
                        {
                            bat 'ctest -C Debug -T test --verbose'
                        }
                    }
                }
            }
        }
    }
}