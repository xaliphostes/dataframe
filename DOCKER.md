# Docker Setup Instructions for Windows

## Prerequisites

### 1. Install Docker Desktop for Windows

1. **Download Docker Desktop**: Go to [https://docs.docker.com/desktop/install/windows-install/](https://docs.docker.com/desktop/install/windows-install/)
2. **Install Docker Desktop**: Run the installer and follow the instructions
3. **Enable WSL 2**: Docker Desktop will prompt you to enable WSL 2 (Windows Subsystem for Linux)
4. **Restart your computer** when prompted

### 2. Verify Docker Installation

Open PowerShell or Command Prompt and run:
```bash
docker --version
docker run hello-world
```

## Building and Running the Dockerfile

### Option 1: Public Repository (No Credentials Needed)

If the repository is public, you can build without GitHub credentials:

1. **Save the corrected Dockerfile** as `Dockerfile` in any directory
2. **Open PowerShell** in that directory
3. **Build the image**:
   ```bash
   docker build -t dataframe:latest .
   ```
4. **Run the container**:
   ```bash
   docker run --rm -it dataframe:latest
   ```

### Option 2: Private Repository (Credentials Required)

If you need GitHub credentials:

1. **Get your GitHub Personal Access Token**:
   - Go to [https://github.com/settings/tokens](https://github.com/settings/tokens)
   - Click "Generate new token" → "Generate new token (classic)"
   - Select scopes: `repo` (for private repos)
   - Copy the token (save it somewhere safe!)

2. **Build with credentials** (replace with your actual values):
   ```bash
   docker build -t dataframe-builder --build-arg GITHUB_USER=fmaerten --build-arg GITHUB_TOKEN=your_token_here .
   ```

3. **Run the container**:
   ```bash
   docker run --rm -it dataframe-builder
   ```

## Usage Examples

### Just Build and Test (Default)
```bash
docker run --rm dataframe-builder
```

### Interactive Shell for Development
```bash
docker run --rm -it dataframe-builder bash
```

### Copy Build Artifacts to Windows
```bash
# Start container
docker run -d --name builder dataframe-builder

# Copy build directory to Windows
docker cp builder:/app/dataframe/build/ ./build/

# Clean up
docker rm builder
```

## Troubleshooting

### Docker Desktop Issues
- **Error: "Docker Desktop requires Windows 10 Pro/Enterprise"**: You need Windows 10 Pro, Enterprise, or Windows 11
- **WSL 2 installation failed**: Enable Windows features: "Windows Subsystem for Linux" and "Virtual Machine Platform"
- **Docker daemon not running**: Start Docker Desktop from the Start menu

### Build Issues
- **GitHub authentication failed**: Check your username and token
- **Out of memory**: In Docker Desktop settings, increase memory allocation to 4GB+
- **Build takes too long**: This is normal for the first build (downloads packages)

### Network Issues
- **Corporate firewall**: May need to configure Docker to use corporate proxy
- **DNS issues**: Try using Google DNS (8.8.8.8) in Docker Desktop network settings

## What the Dockerfile Does

1. **Sets up Ubuntu 22.04** with modern C++ tools
2. **Installs dependencies**: GCC 12, CMake, Eigen3, CGAL, Boost
3. **Clones your repository** from GitHub
4. **Builds the project** with CMake (Release mode, C++17, CGAL enabled)
5. **Runs all tests** using ctest
6. **Provides interactive shell** for further development

## File Structure After Build

Inside the container, you'll find:
```
/app/dataframe/          # Your repository
├── include/             # Header files
├── build/               # Compiled binaries and test results
│   ├── tests/           # Test executables
│   └── ...
└── CMakeLists.txt       # Build configuration
```

## Next Steps

Once the container runs successfully:
- All tests will have passed
- You'll have a verified Linux build of your library
- You can copy the built artifacts back to Windows if needed
- Use the container for continuous integration or development