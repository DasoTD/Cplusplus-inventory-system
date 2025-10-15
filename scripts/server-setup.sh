#!/bin/bash
# Server Setup Script for C++ Inventory System
# Run this on your DigitalOcean droplet first

set -e

echo "🚀 Setting up server for C++ Inventory System deployment..."

# Update system
echo "📦 Updating system packages..."
sudo apt-get update
sudo apt-get upgrade -y

# Install essential packages
echo "🔧 Installing essential packages..."
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    nginx \
    certbot \
    python3-certbot-nginx \
    htop \
    curl \
    wget \
    unzip \
    libssl-dev \
    zlib1g-dev \
    libjsoncpp-dev \
    uuid-dev \
    sqlite3 \
    libsqlite3-dev \
    libbrotli-dev \
    ninja-build \
    ufw

# Create www-data user directories
echo "👤 Setting up www-data user..."
sudo mkdir -p /opt/inventory_system
sudo chown www-data:www-data /opt/inventory_system

# Install Drogon framework
echo "🌐 Installing Drogon framework..."
if [ ! -d "/tmp/drogon" ]; then
    cd /tmp
    git clone https://github.com/drogonframework/drogon.git
    cd drogon
    git checkout v1.9.6
    git submodule update --init
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -G Ninja
    ninja -j$(nproc)
    sudo ninja install
    sudo ldconfig
    echo "✅ Drogon installed successfully"
else
    echo "✅ Drogon already installed"
fi

# Configure firewall
echo "🔥 Configuring firewall..."
sudo ufw --force reset
sudo ufw default deny incoming
sudo ufw default allow outgoing
sudo ufw allow ssh
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp
sudo ufw allow 7777/tcp  # For direct access during testing
sudo ufw --force enable

# Configure Nginx
echo "🌐 Configuring Nginx..."
sudo systemctl enable nginx
sudo systemctl start nginx

# Create a basic Nginx config (will be replaced by deployment)
sudo tee /etc/nginx/sites-available/inventory-system > /dev/null << 'EOF'
server {
    listen 80 default_server;
    listen [::]:80 default_server;
    
    server_name _;
    
    location / {
        proxy_pass http://127.0.0.1:7777;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
    }
    
    location /health {
        proxy_pass http://127.0.0.1:7777/health;
        access_log off;
    }
}
EOF

# Enable the site
sudo rm -f /etc/nginx/sites-enabled/default
sudo ln -sf /etc/nginx/sites-available/inventory-system /etc/nginx/sites-enabled/
sudo nginx -t && sudo systemctl reload nginx

# Set up log rotation
echo "📝 Setting up log rotation..."
sudo tee /etc/logrotate.d/inventory-system > /dev/null << 'EOF'
/var/log/inventory-system/*.log {
    daily
    rotate 14
    compress
    delaycompress
    missingok
    notifempty
    create 644 www-data www-data
    postrotate
        systemctl reload inventory-system
    endscript
}
EOF

# Create log directory
sudo mkdir -p /var/log/inventory-system
sudo chown www-data:www-data /var/log/inventory-system

# Display server information
echo ""
echo "🎉 Server setup complete!"
echo ""
echo "📋 Server Information:"
echo "   - OS: $(lsb_release -d | cut -f2)"
echo "   - IP: $(curl -s ifconfig.me)"
echo "   - Drogon: v1.9.6"
echo "   - Nginx: $(nginx -v 2>&1 | cut -d'/' -f2)"
echo ""
echo "🔧 Next Steps:"
echo "1. Set up GitHub secrets for deployment:"
echo "   - DEPLOY_HOST: $(curl -s ifconfig.me)"
echo "   - DEPLOY_USER: $(whoami)"
echo "   - DEPLOY_KEY: Your SSH private key"
echo "   - DEPLOY_PORT: 22 (default)"
echo ""
echo "2. Optional - Set up domain and SSL:"
echo "   - Point your domain to: $(curl -s ifconfig.me)"
echo "   - Run: sudo certbot --nginx -d your-domain.com"
echo ""
echo "3. Test deployment:"
echo "   - Push to main branch to trigger deployment"
echo "   - Check: http://$(curl -s ifconfig.me):7777/health"
echo ""
echo "📚 Useful commands:"
echo "   - Check service: sudo systemctl status inventory-system"
echo "   - View logs: sudo journalctl -u inventory-system -f"
echo "   - Restart service: sudo systemctl restart inventory-system"
echo "   - Check Nginx: sudo nginx -t"
echo ""