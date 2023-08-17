#ifndef __DEAL_H__
#define __DEAL_H__

#include <memory>
#include <unordered_map>

#include "catalogue.h"
#include "command.h"
#include "inventory.h"
#include "observer.h"

namespace catapult {

//! @brief represents a reduction in cost
class Discount {
  public:
    virtual ~Discount () = default;
    //! returns the value which should be subtracted from the gross total in order to effect this discount
    virtual uint32_t getValue () const = 0;
};

//! @brief represents a reduction in cost on the basis of multiple purchases of one or more products
class MultiplePurchaseDiscount: public Discount {
  public:
    MultiplePurchaseDiscount (const Product &product, uint32_t quantity):
      _product { product }, _quantity { quantity } {};
    uint32_t getValue () const override;

  private:
    Product _product;
    uint32_t _quantity;
};

//! @brief represents a means of calculating discounts
class Deal {
  public:
    virtual ~Deal () = default;
    virtual std::vector<std::shared_ptr<Discount>> getDiscounts () const = 0;
};

//! @brief reprensents a means of calculating discounts on the basis of observed StockChangeMessages
class MultiplePurchaseDeal: public Deal, public Observer<StockChangeMessage> {
  public:
    //! @brief constructor
    //! @param eligableProducts a vector of products for which this discount is eligable
    MultiplePurchaseDeal (const std::vector<Product> &eligableProducts);


    void notify (StockChangeMessage) override;
    std::vector<std::shared_ptr<Discount>> getDiscounts () const override;

  protected:
    //! @brief returns the total number of products which are eligable for discount in this instance
    uint32_t size () const;

  private:
    std::unordered_map<Product, uint32_t> _eligableProducts;
    uint32_t _multiplicity { 3 };
};


//! @brief stores all available deals
class DealRegistry {
  public:
    virtual ~DealRegistry () = default;

    //! @brief returns a vector of deals associated with a product
    virtual std::vector<std::shared_ptr<Deal>> getDeals (const Product &product) const = 0;

    template<class ConcreteDeal>
    class RegisterDealCommand;

  protected:
    virtual void _registerDeal (std::shared_ptr<Deal> deal) = 0;
};


//! Used to register new deals with the deal registry
template<class ConcreteDeal>
class DealRegistry::RegisterDealCommand: public Command<DealRegistry> {
  public:
    RegisterDealCommand (ConcreteDeal deal): _deal { deal } {};

  protected:
    void _execute (DealRegistry &dealRegistry) const override {
      dealRegistry._registerDeal(std::make_shared<ConcreteDeal>(_deal));
    };

  private:
    ConcreteDeal _deal;
};


}  // namespace catapult

#endif  // __DEAL_H__
