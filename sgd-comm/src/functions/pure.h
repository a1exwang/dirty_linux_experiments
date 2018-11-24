#pragma once
#include <tensor.h>
#include <functions/function.h>
#include <functional>


namespace asgd {

class PureFunction :public Function {
public:
  PureFunction(const std::string &name, int64_t n, int64_t bs,
               std::function<Dtype(Dtype)> fn, std::function<Dtype(Dtype, Dtype)> derivative)
      :Function(name, {}, {bs, n}), n(n), fn(fn), derivative(derivative) { }

  const Tensor &operator() (const Tensor &input) override {
    int64_t bs = input.shape()[0];
    for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
      for (int i = 0; i < n; i++) {
        output_[i_bs*n + i] = fn(input[i_bs*n + i]);
      }
    }
    return output_;
  }
  void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) override {
    for (int64_t i = 0; i < n; i++) {
      input_diff[i] = this->derivative(input[i], output_[i]) * output_diff[i];
    }
  }
private:
  virtual bool learnable() const { return false; };
  // weight is (n_in+1) * n_out
  int64_t n;
  std::function<Dtype(Dtype)> fn;
  std::function<Dtype(Dtype, Dtype)> derivative;
};

}
