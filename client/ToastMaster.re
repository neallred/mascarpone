let css = ReactDOMRe.Style.make;
let cx = ReactDOMRe.Style.combine;
let str = ReasonReact.string;
let _0 = ReasonReact.null;
let multi = (x) => x |> Array.of_list |> ReasonReact.array;

module SMap = Map.Make(String);

let emptyToasts: SMap.t(Toast.toastDetails) = SMap.empty;
let errorToasts: SMap.t(Toast.toastDetails) = SMap.empty
  |> SMap.add(
    "error-adding-domain",
    {
      Toast.type_: Toast.Failure,
      message: "Could not add \"adsf.asdf\"",
      time_of_appearance: Js.Date.now()
    }
  )
  |> SMap.add(
    "error-updating-domain",
    {
      Toast.type_: Toast.Failure,
      message: "Could not change \"adsf.asdf\"",
      time_of_appearance: Js.Date.now()
    }
  )
  |> SMap.add(
    "error-deleting-domain",
    {
      Toast.type_: Toast.Failure,
      message: "Unable to remove \"adsf.asdf\"",
      time_of_appearance: Js.Date.now()
    }
  );

[@react.component]
let make = (
  ~toasts: SMap.t(Toast.toastDetails),
  ~clearToast,
) => {
  let toastList: list(React.element) = SMap.bindings(toasts)
    |> List.stable_sort(((_, toastA: Toast.toastDetails), (_, toastB)) => {
      int_of_float(toastA.time_of_appearance -. toastB.time_of_appearance)
    })
    |> List.map(((toastKey, toast)) => {
      <Toast
        key={toastKey}
        toast
        toastKey
        clearToast
      />
    });

  <div>
    {multi(toastList)}
  </div>
};
