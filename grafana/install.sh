sudo yum install -y https://dl.grafana.com/oss/release/grafana-9.5.2-1.x86_64.rpm
sudo yum install grafana
sudo systemctl start grafana-server

https://grafana.com/docs/grafana/latest/setup-grafana/installation/redhat-rhel-fedora/
https://grafana.com/docs/grafana/latest/setup-grafana/start-restart-grafana/

sudo systemctl start nginx
vim /etc/nginx/nginx.conf
location / {
    proxy_set_header Host $host;
    proxy_pass http://127.0.0.1:3000/;
}   