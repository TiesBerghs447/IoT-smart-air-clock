FROM php:8.2-apache

COPY software/php/ /var/www/html/

EXPOSE 80
