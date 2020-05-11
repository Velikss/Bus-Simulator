pipeline {
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
                            sh 'cmake .'
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
                            bat 'cmake . -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake'
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
                            bat 'ctest -T test --verbose'
                        }
                    }
                }
            }
        }
    }
}