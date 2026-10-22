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
        <section class="login-card" aria-label="Inloggen bij Smart Air Clock">
            <div class="login-showcase" aria-hidden="true">
                <div class="login-showcase-top">
                    <span class="login-brand-mark">
                        <svg viewBox="0 0 48 48" fill="none">
                            <path d="M8 18h21a6 6 0 1 0-6-6" />
                            <path d="M6 25h29a5 5 0 1 1-5 5" />
                            <path d="M10 32h12a4 4 0 1 1-4 4" />
                        </svg>
                    </span>
                    <span>SMART AIR CLOCK</span>
                </div>

                <div class="login-orbit">
                    <span class="login-orbit-ring login-orbit-ring-outer"></span>
                    <span class="login-orbit-ring login-orbit-ring-inner"></span>
                    <span class="login-orbit-dot"></span>
                    <span class="login-orbit-core">
                        <svg viewBox="0 0 48 48" fill="none">
                            <path d="M8 18h21a6 6 0 1 0-6-6" />
                            <path d="M6 25h29a5 5 0 1 1-5 5" />
                            <path d="M10 32h12a4 4 0 1 1-4 4" />
                        </svg>
                    </span>
                </div>

                <div class="login-showcase-copy">
                    <span class="login-eyebrow">JOUW BINNENKLIMAAT</span>
                    <h2>Een frisse blik<br>op binnenlucht.</h2>
                    <p>Alle inzichten en instellingen van jouw Smart Air Clock op één plek.</p>
                </div>

                <div class="login-showcase-footer">
                    <span class="login-status-dot"></span>
                    Slimme lucht. Helder inzicht.
                </div>
            </div>

            <div class="login-panel">
                <div class="login-panel-heading">
                    <span class="login-secure-badge">
                        <svg viewBox="0 0 20 20" fill="none" aria-hidden="true">
                            <rect x="4.5" y="8.5" width="11" height="8" rx="2" />
                            <path d="M7 8.5V6a3 3 0 1 1 6 0v2.5" />
                        </svg>
                        BEVEILIGDE TOEGANG
                    </span>
                    <h1>Welkom terug<span>.</span></h1>
                    <p>Meld je aan om verder te gaan naar je dashboard.</p>
                </div>

                <?php if ($errorMessage !== null): ?>
                    <div class="login-error" role="alert">
                        <span class="login-error-icon" aria-hidden="true">!</span>
                        <?= htmlspecialchars($errorMessage, ENT_QUOTES, 'UTF-8'); ?>
                    </div>
                <?php endif; ?>

                <form method="post" class="login-form">
                    <input
                        type="hidden"
                        name="csrf_token"
                        value="<?= htmlspecialchars($csrfToken, ENT_QUOTES, 'UTF-8'); ?>">

                    <label for="username">Gebruikersnaam</label>
                    <div class="login-input-wrap">
                        <svg viewBox="0 0 24 24" fill="none" aria-hidden="true">
                            <circle cx="12" cy="8" r="3.5" />
                            <path d="M5 20a7 7 0 0 1 14 0" />
                        </svg>
                        <input
                            id="username"
                            type="text"
                            name="username"
                            placeholder="Je gebruikersnaam"
                            autocomplete="username"
                            required
                            autofocus>
                    </div>

                    <label for="passcode">Wachtwoord</label>
                    <div class="login-input-wrap">
                        <svg viewBox="0 0 24 24" fill="none" aria-hidden="true">
                            <rect x="4.5" y="10" width="15" height="10" rx="2.5" />
                            <path d="M8 10V7a4 4 0 1 1 8 0v3" />
                            <path d="M12 14v2" />
                        </svg>
                        <input
                            id="passcode"
                            type="password"
                            name="passcode"
                            placeholder="Je wachtwoord"
                            autocomplete="current-password"
                            required>
                    </div>

                    <button class="login-submit" type="submit">
                        <span>Inloggen</span>
                        <svg viewBox="0 0 24 24" fill="none" aria-hidden="true">
                            <path d="M5 12h14M13 6l6 6-6 6" />
                        </svg>
                    </button>
                </form>

                <p class="login-footnote">Je gegevens worden veilig en privé verwerkt.</p>
            </div>
        </section>
    </main>
</body>
</html>
