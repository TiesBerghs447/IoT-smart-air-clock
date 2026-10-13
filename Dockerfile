FROM php:8.2-apache

# Installeert databaseondersteuning en maakt de runtime-mappen schrijfbaar
# waar PHP de sensorcommando's en JSON-instellingen opslaat.
RUN apt-get update && apt-get install -y libpq-dev
RUN docker-php-ext-install pdo pdo_pgsql

COPY software/php/ /var/www/html/
RUN chown www-data:www-data /var/www/html/data
RUN chown www-data:www-data /var/www/html/settings /var/www/html/settings/config.json

EXPOSE 80