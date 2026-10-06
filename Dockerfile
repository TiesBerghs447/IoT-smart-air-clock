FROM php:8.2-apache

COPY software/php/data/ /var/www/html/

EXPOSE 80
