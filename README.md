# Catapult Checkout

## Brief

We’d like you to design a supermarket Checkout that calculates the total price of a number of
items.

Items are stored with their unit price.
Some items are eligible for discounts based on the following 2 deal types:
- buy 3 identical items and pay for 2
- buy 3 (in a set of items) and the cheapest is free
No other deal types need to be considered for the purposes of this exercise.

The output from the C++ program should be the customer Receipt. This should show the price of
each Item bought and the grand total.

You may choose any sensible means of accepting input and producing output.

## Assumptions

- In addition to the final total, we also want an output of the gross price (price before promotions are applied).
- Items are only ever priced on discrete units of products, rather than prices calculated by weight, e.g.
- Not all items are eligible for discounts
  - Discount eligibility will be stored along with the price of each item.
- If one item is included in one discount, it cannot be included as a part of another.
  - An item cannot be included in more than one type of promotion.
- If an item is included in the buy 3 (in a set of items), then multiple purchases of the same item *will* contribute towards that promotion
- For clarity, we define: An *Item* **IS A** *product* **PART OF** A *group*.
- The action of adding an item to a cart is irreversible (FOR NOW).
- The receipt should be written to a file for printing.
