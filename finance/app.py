import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    id = session["user_id"]
    portfolio = db.execute(
        "SELECT symbol, SUM(quantity) FROM transactions GROUP BY symbol HAVING id = ? AND SUM(quantity) != 0", id)
    cash = db.execute("SELECT cash FROM users WHERE id = ?", id)
    cash_total = float(cash[0]['cash'])
    total = cash_total

    for stock in portfolio:
        stock_data = lookup(stock["symbol"])
        stock["price"] = stock_data["price"]
        stock["value"] = stock["price"] * int(stock["SUM(quantity)"])
        total += stock["value"]

    return render_template("index.html", portfolio=portfolio, cash_total=cash_total, total=total)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "GET":
        return render_template("buy.html")

    symbol = request.form.get("symbol")
    if not symbol:
        return apology("Symbol cannot be blank")

    # Retrieve symbol data
    symbol_data = lookup(symbol)
    if not symbol_data:
        return apology("Symbol not found")

    # Retrieve quantity to buy
    try:
        quantity = int(request.form.get("shares"))
    except TypeError:
        return apology("Quantity must be a number")
    except ValueError:
        return apology("Quantity must be a whole number")
    else:
        if quantity < 1:
            return apology("Quantity must be more than 0")

    price = symbol_data["price"]
    id = session["user_id"]
    cash = db.execute("SELECT cash FROM users WHERE id = ?", id)[0]['cash']

    if 0 < price * quantity <= cash:
        date = db.execute("SELECT datetime('now','localtime')")[0]["datetime('now','localtime')"]
        db.execute("INSERT INTO transactions (id, symbol, price, quantity, date, type) VALUES (?, ?, ?, ?, ?, ?)",
                   id,
                   symbol_data["symbol"],
                   price,
                   quantity,
                   date,
                   "Buy"
                   )
        db.execute("UPDATE users SET cash = ? WHERE id = ?", cash - (price * quantity), id)
        return redirect("/")

    return apology("You can't afford this!")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    transaction_history = db.execute(
        "SELECT type, symbol, price, quantity, date FROM transactions WHERE id = ?", session['user_id'])

    return render_template("history.html", transaction_history=transaction_history)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    # Render quote page
    if request.method == "GET":
        return render_template("quote.html")

    # Get symbol from user form
    symbol = request.form.get("symbol")
    if not symbol:
        return apology("Symbol cannot be blank")

    # Retrieve symbol data
    quote_data = lookup(symbol)
    if not quote_data:
        return apology("Symbol not found")

    # Display quote for user's requested symbol
    return render_template("quoted.html", name=quote_data["name"], symbol=quote_data["symbol"], price=usd(quote_data["price"]))


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":
        users = db.execute("SELECT username FROM users")
        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        # Ensure username is not blank
        if not username:
            return apology("username cannot be blank")

        # Check if username already exists
        for user in users:
            if username == user["username"]:
                return apology("username already exists")

        # Ensure passwords are not blank
        if not (password or confirmation):
            return apology("password(s) cannot be blank")

        # Ensure passwords match
        if password != confirmation:
            return apology("password(s) do not match")

        # Hash password, then add username and hash to users table
        hashed_password = generate_password_hash(password)
        db.execute("INSERT INTO users (username, hash) VALUES (?, ?)", username, hashed_password)

        # Remember which user has logged in
        session["user_id"] = db.execute("SELECT id FROM users WHERE username = ?", username)[0]['id']

        # Redirect user to home page
        return redirect("/")

    # Request made was GET, as if clicking the Register link
    return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    id = session["user_id"]
    portfolio = db.execute(
        "SELECT symbol, SUM(quantity) FROM transactions GROUP BY symbol HAVING id = ? AND SUM(quantity) != 0", id)

    if request.method == "GET":
        return render_template("sell.html", portfolio=portfolio)

    symbol = request.form.get("symbol")

    try:
        shares = int(request.form.get("shares"))
    except TypeError:
        return apology("Quantity must be a number")
    except ValueError:
        return apology("Quantity must be a whole number")

    symbols = [symbol['symbol'] for symbol in portfolio if 'symbol' in symbol]

    # Symbol left blank
    if not symbol:
        return apology("Symbol wasn't selected")

    # Symbol not owned
    if symbol not in symbols:
        return apology("You don't own this stock!")

    if shares <= 0:
        return apology("You can only sell at least 1 share")

    # Get quantity of shares held
    symbol_quantity = 0
    for s in portfolio:
        if s['symbol'] == symbol:
            symbol_quantity = s['SUM(quantity)']
            break

    if shares > symbol_quantity:
        return apology(f"You don't have that many {symbol} shares.")

    # Sell shares and update tables
    sold_price = lookup(symbol)['price']
    date = db.execute("SELECT datetime('now','localtime')")[0]["datetime('now','localtime')"]
    db.execute(
        "INSERT INTO transactions (id, symbol, price, quantity, date, type) VALUES (?, ?, ?, ?, ?, ?)", id, symbol, sold_price, -shares, date, "Sell")
    db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", sold_price * shares, id)

    # Return to homepage
    return redirect("/")


@app.route("/account", methods=["GET", "POST"])
@login_required
def account():
    """Show change password dialog and allow change of password"""

    if request.method == "GET":
        return render_template("account.html")

    # POST
    hash = db.execute("SELECT hash FROM users WHERE id = ?", session["user_id"])[0]['hash']
    if not check_password_hash(hash, request.form.get("password")):
        return apology("Your password is incorrect.")

    new_password = request.form.get("new_password")
    confirmation = request.form.get("confirmation")
    if new_password != confirmation:
        return apology("New password doesn't match confirmation")

    if not new_password:
        return apology("Password cannot be blank!")

    # Set new password and return to homepage
    db.execute("UPDATE users SET hash = ? WHERE id = ?", generate_password_hash(new_password), session["user_id"])
    return redirect("/")