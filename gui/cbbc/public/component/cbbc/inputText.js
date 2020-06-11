class InputText extends React.Component {
  static propTypes = {
    no: PropTypes.string,
    label: PropTypes.string,
    name: PropTypes.string,
    value: PropTypes.string,
    feedback: PropTypes.string,
    placeholder: PropTypes.string,
    disabled: PropTypes.bool,
    onChange: PropTypes.func,
  }
  
  render() {
    var style = (this.props.feedback ? "is-invalid" : "")
    return(
      <div className="form-group row">
        <label className="col-5 col-sm-5 col-form-label col-form-label-sm"> {this.props.label} </label>
        <div className="col-7 col-sm-7"> 
          <input 
            type="text"
            className={classNames('form-control form-control-sm', style)}
            data-no={this.props.no}
            name={this.props.name}
            value={this.props.value}
            disabled={this.props.disabled}
            onChange={this.props.onChange} 
            placeholder={this.props.placeholder} />
        </div>
        <div className="col-5 col-sm-5"> </div>
        <div className="col-7 col-sm-7 invalid-feedback d-block"> {this.props.feedback} </div>
      </div>
    )
  }
}