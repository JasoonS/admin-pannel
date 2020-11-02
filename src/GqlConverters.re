[@decco]
type stringArray = array(string);

module StringArray = {
  type t = array(string);
  let parse = json =>
    json->stringArray_decode->Belt.Result.getWithDefault([||]);
  let serialize = array => array->stringArray_encode;
};
