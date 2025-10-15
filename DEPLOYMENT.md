# 🚀 Deployment Guide

This guide will help you deploy your C++ Inventory System to a DigitalOcean droplet with automated CI/CD deployment.

## 📋 Prerequisites

- DigitalOcean account (or any Ubuntu VPS)
- Domain name (optional, but recommended)
- GitHub repository with push access

## 🖥️ Server Setup

### 1. Create DigitalOcean Droplet

1. **Create Droplet:**
   - OS: Ubuntu 22.04 LTS
   - Size: Basic $6/month (1GB RAM, 1 vCPU) minimum
   - Add your SSH key

2. **Connect to server:**
   ```bash
   ssh root@your-server-ip
   ```

3. **Run setup script:**
   ```bash
   wget https://raw.githubusercontent.com/YOUR_USERNAME/Cplusplus-inventory-system/main/scripts/server-setup.sh
   chmod +x server-setup.sh
   ./server-setup.sh
   ```

## 🔐 GitHub Secrets Setup

Add these secrets to your GitHub repository:

1. Go to: `Settings` → `Secrets and variables` → `Actions`
2. Add these secrets:

| Secret Name | Value | Description |
|-------------|-------|-------------|
| `DEPLOY_HOST` | `your-server-ip` | Server IP address |
| `DEPLOY_USER` | `root` or `ubuntu` | SSH username |
| `DEPLOY_KEY` | `-----BEGIN OPENSSH PRIVATE KEY-----...` | SSH private key |
| `DEPLOY_PORT` | `22` | SSH port (optional) |

### 🔑 Getting SSH Private Key

```bash
# On your local machine (not server)
cat ~/.ssh/id_rsa
# Copy entire output including BEGIN/END lines
```

## 🌐 Domain Setup (Optional)

### 1. Point Domain to Server
- Add A record: `@` → `your-server-ip`
- Add A record: `www` → `your-server-ip`

### 2. Setup SSL Certificate
```bash
# On your server
sudo certbot --nginx -d your-domain.com -d www.your-domain.com
```

### 3. Update Nginx Config
Edit `/etc/nginx/sites-available/inventory-system`:
```nginx
server_name your-domain.com www.your-domain.com;
```

## 🚀 Deployment Process

### Automatic Deployment
1. **Push to main branch:**
   ```bash
   git push origin main
   ```

2. **Monitor deployment:**
   - Check GitHub Actions tab
   - Watch for green checkmark ✅

### Manual Deployment
```bash
# Trigger manual deployment
# Go to Actions → Deploy to Production → Run workflow
```

## 📊 Monitoring & Management

### Check Application Status
```bash
# Service status
sudo systemctl status inventory-system

# View logs
sudo journalctl -u inventory-system -f

# Restart service
sudo systemctl restart inventory-system
```

### Check Web Server
```bash
# Test Nginx
sudo nginx -t

# Reload Nginx
sudo systemctl reload nginx

# Check if app is responding
curl http://localhost:7777/health
```

### Resource Monitoring
```bash
# System resources
htop

# Disk usage
df -h

# Service memory usage
sudo systemctl status inventory-system
```

## 🔧 Troubleshooting

### Common Issues

**1. Service won't start:**
```bash
# Check logs
sudo journalctl -u inventory-system -n 50

# Check file permissions
ls -la /opt/inventory_system/

# Fix permissions
sudo chown -R www-data:www-data /opt/inventory_system
```

**2. Nginx errors:**
```bash
# Check Nginx error log
sudo tail -f /var/log/nginx/error.log

# Test configuration
sudo nginx -t
```

**3. Deployment fails:**
```bash
# Check SSH connection
ssh -i ~/.ssh/your-key user@server-ip

# Verify secrets in GitHub
# Re-run deployment workflow
```

### Application URLs

After successful deployment:

- **Direct access:** `http://your-server-ip:7777`
- **Through Nginx:** `http://your-server-ip` or `http://your-domain.com`
- **Health check:** `http://your-server-ip:7777/health`

## 🔄 Updates & Maintenance

### Automatic Updates
- Every push to `main` branch triggers deployment
- Zero-downtime deployment with systemd restart

### Manual Updates
```bash
# SSH to server
ssh user@your-server-ip

# Restart application
sudo systemctl restart inventory-system

# Update system packages
sudo apt update && sudo apt upgrade
```

### Backup Database
```bash
# If using SQLite (default)
sudo cp /opt/inventory_system/*.db /backup/location/

# If using external database
# Use appropriate backup commands
```

## 📈 Scaling & Performance

### Vertical Scaling (More Resources)
1. Resize droplet in DigitalOcean panel
2. Reboot server
3. No configuration changes needed

### Horizontal Scaling (Multiple Servers)
1. Set up load balancer
2. Use external database (PostgreSQL/MySQL)
3. Update connection strings

### Performance Optimization
```bash
# Enable compression in Nginx
# Add to server block:
gzip on;
gzip_vary on;
gzip_proxied any;
gzip_comp_level 6;
gzip_types text/plain text/css application/json application/javascript text/xml application/xml;
```

## 🆘 Support

- **Server issues:** Check DigitalOcean documentation
- **Application issues:** Check GitHub Actions logs
- **SSL issues:** Check Certbot documentation

---

## 📋 Quick Reference

```bash
# Essential commands
sudo systemctl status inventory-system    # Check status
sudo journalctl -u inventory-system -f    # View logs
sudo systemctl restart inventory-system   # Restart app
sudo nginx -t                            # Test Nginx config
curl http://localhost:7777/health         # Health check
```

🎉 **Your C++ application is now production-ready with automated deployment!**