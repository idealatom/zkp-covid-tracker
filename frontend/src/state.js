export const ADD_PROOF = 'ADD_PROOF';
export const PENDING = 'PENDING';

export const initialState = {
  proofs: [],
  lastProof: null,
};

export const addProof = (proof) => ({
  type: ADD_PROOF,
  payload: proof,
});

export const setPending = () => ({
  type: PENDING,
});

export const proofListReducer = (state = initialState, action) => {
  if (action.type === ADD_PROOF) {
    return {
      ...state,
      proofs: action.payload
        ? [...new Set(state.proofs.concat(action.payload))]
        : state.proofs,
      lastProof: action.payload,
      pending: false,
    };
  } else if (action.type === PENDING) {
    return {
      ...state,
      pending: true,
    };
  }
  return state;
};
