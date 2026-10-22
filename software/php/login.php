<?php

require_once __DIR__ . '/settings/web_auth.php';
web_auth_start_session();

if (!empty($_SESSION['web_authenticated'])) {
    header('Location: ' . web_auth_app_path() . '/index.php');
    exit;
}

$errorMessage = null;
$csrfToken = web_auth_csrf_token();

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = getenv('user');
    $passcode = getenv('passcode');

    if (
        !is_string($username)
        || $username === ''
        || !is_string($passcode)
        || $passcode === ''
    ) {
        http_response_code(503);
        $errorMessage = 'De login is nog niet ingesteld op de server. Controleer de Render-variabelen user en passcode.';
    } elseif (!web_auth_validate_csrf()) {
        http_response_code(400);
        $errorMessage = 'De sessie is verlopen. Vernieuw de pagina en probeer opnieuw.';
    } else {
        $submittedUsername = $_POST['username'] ?? '';
        $submittedPasscode = $_POST['passcode'] ?? '';

        if (
            is_string($submittedUsername)
            && is_string($submittedPasscode)
            && hash_equals($username, $submittedUsername)
            && hash_equals($passcode, $submittedPasscode)
        ) {
            session_regenerate_id(true);
            $_SESSION['web_authenticated'] = true;

            $returnTo =
                $_SESSION['web_auth_return_to'] ?? '';
            unset($_SESSION['web_auth_return_to']);

            $appPath = web_auth_app_path();
            $returnPath = is_string($returnTo)
                ? parse_url($returnTo, PHP_URL_PATH)
                : null;

            if (
                !is_string($returnPath)
                || !str_starts_with($returnPath, $appPath . '/')
                || str_starts_with($returnPath, '//')
            ) {
                $returnTo = $appPath . '/index.php';
            }

            header('Location: ' . $returnTo);
            exit;
        }

        http_response_code(401);
        $errorMessage = 'Gebruikersnaam of wachtwoord is onjuist.';
    }
}
?>
<!DOCTYPE html>
<html lang="nl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Inloggen - Smart Air Clock</title>
    <link rel="stylesheet" href="stylesheet.css">
</head>
<body>
    <main class="login-container">
        <section class="settings-card login-card">
            <div class="logo">🌬 Smart Air Clock</div>
            <h1>Inloggen</h1>
            <p class="subtitle">Log in om de website te bekijken.</p>

            <?php if ($errorMessage !== null): ?>
                <div class="error" role="alert">
                    <?= htmlspecialchars($errorMessage, ENT_QUOTES, 'UTF-8'); ?>
                </div>
            <?php endif; ?>

            <form method="post" class="login-form">
                <input
                    type="hidden"
                    name="csrf_token"
                    value="<?= htmlspecialchars($csrfToken, ENT_QUOTES, 'UTF-8'); ?>">
                <label for="username">Gebruikersnaam</label>
                <input
                    id="username"
                    type="text"
                    name="username"
                    autocomplete="username"
                    required
                    autofocus>

                <label for="passcode">Wachtwoord</label>
                <input
                    id="passcode"
                    type="password"
                    name="passcode"
                    autocomplete="current-password"
                    required>

                <button class="save-btn" type="submit">
                    Inloggen
                </button>
            </form>
        </section>
    </main>
</body>
</html>
