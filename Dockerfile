FROM php:8.2-apache

RUN docker-php-ext-install mysqli

COPY software/php/ /var/www/html/

EXPOSE 80